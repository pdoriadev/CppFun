#include "RayTracer.h"
#include "TextureCache.h"

#include "glad/glad.h"

#include "Shape.h"
#include "ShapeManager.h"
#include "SceneLight.h"
#include "ImageIO.h"

#include <chrono>
#include <cmath>
#include <algorithm>

namespace {

inline unsigned char toByte(float v) {
    // Clamp then gamma-free 8-bit quantize. The rasterized viewport writes
    // linear values straight out too, so staying linear keeps the two matching.
    //
    // WRITTEN AS !(v > 0) RATHER THAN v <= 0 SO THAT NaN LANDS HERE.
    //
    // Every comparison against NaN is false, so the obvious spelling let a NaN
    // fall past both guards into static_cast<unsigned char>(NaN), which is
    // undefined behaviour:
    //
    //     runtime error: nan is outside the range of representable values
    //                    of type 'unsigned char'
    //
    // It happens to yield 0 on x86 and could yield anything elsewhere. And a
    // NaN colour is not exotic -- it is one degenerate triangle away. A
    // student's cylindrical UVs divide by a mesh's own extent, their Phong
    // normalises a vector that can be zero-length, and Texture::sample takes
    // whatever UV it is handed. Any of those produces NaN, and the pixel writer
    // is given code that must survive it.
    //
    // Black is the right answer for "not a number": visible, harmless, and it
    // makes the bad pixels obvious against a rendered image.
    if (!(v > 0.0f)) return 0;      // catches NaN and everything <= 0
    if (v >= 1.0f) return 255;
    return static_cast<unsigned char>(v * 255.0f + 0.5f);
}

double nowSeconds() {
    using namespace std::chrono;
    return duration_cast<duration<double> >(steady_clock::now().time_since_epoch()).count();
}

} // namespace

RayTracer::RayTracer()
    : nextRow(0), rendering(false), elapsed(0.0),
      previewTexture(0), textureDirty(false) {}

RayTracer::~RayTracer() {
    // Note: the GL context may already be gone at static teardown. Callers that
    // care should cancel and release before shutdown; we avoid touching GL here.
}

void RayTracer::begin(const Camera& cam, ShapeManager& shapeManager,
                      const RayTraceSettings& s) {
    settings = s;
    if (settings.width  < 1) settings.width  = 1;
    if (settings.height < 1) settings.height = 1;
    if (settings.samplesPerPixel < 1) settings.samplesPerPixel = 1;
    if (settings.maxDepth < 0) settings.maxDepth = 0;

    camera = cam;

    // Flatten the scene now, so later edits mid-render can't corrupt the image.
    scene.build(shapeManager);

    pixels.assign(static_cast<size_t>(settings.width) * settings.height * 3, 0);
    nextRow      = 0;
    rendering    = true;
    elapsed      = 0.0;
    textureDirty = true;
}

void RayTracer::cancel() {
    rendering = false;
}

float RayTracer::progress() const {
    if (settings.height <= 0) return 0.0f;
    return static_cast<float>(nextRow) / static_cast<float>(settings.height);
}

bool RayTracer::step(double budgetSeconds) {
    if (!rendering) return false;

    const double start = nowSeconds();
    const int    spp   = settings.samplesPerPixel;
    const float  invSpp = 1.0f / static_cast<float>(spp * spp);

    while (nextRow < settings.height) {
        const int y = nextRow;

        for (int x = 0; x < settings.width; ++x) {
            glm::vec3 accum(0.0f);

            // Uniform grid supersampling. spp == 1 collapses to one ray through
            // the pixel center.
            for (int sy = 0; sy < spp; ++sy) {
                for (int sx = 0; sx < spp; ++sx) {
                    float ox = (static_cast<float>(sx) + 0.5f) / static_cast<float>(spp);
                    float oy = (static_cast<float>(sy) + 0.5f) / static_cast<float>(spp);

                    Ray ray = camera.generateRay(static_cast<float>(x) + ox,
                                                 static_cast<float>(y) + oy,
                                                 settings.width, settings.height);
                    accum += traceRay(ray, 0);
                }
            }

            glm::vec3 color = accum * invSpp;
            size_t i = (static_cast<size_t>(y) * settings.width + x) * 3;
            pixels[i + 0] = toByte(color.r);
            pixels[i + 1] = toByte(color.g);
            pixels[i + 2] = toByte(color.b);
        }

        ++nextRow;
        textureDirty = true;

        if (nowSeconds() - start >= budgetSeconds) break;
    }

    elapsed += nowSeconds() - start;

    if (nextRow >= settings.height) {
        rendering = false;
        return false;
    }
    return true;
}

glm::vec3 RayTracer::traceRay(const Ray& ray, int depth) const {
    Hit hit;
    if (!scene.intersect(ray, 1e-4f, 1e30f, hit)) {
        return settings.background;
    }
    return shade(hit, ray, depth);
}

bool RayTracer::saveImage(const std::string& path) const {
    if (pixels.empty()) return false;
    return ImageIO::write(path, pixels, settings.width, settings.height);
}

void RayTracer::discardImage() {
    cancel();
    destroyTexture();
    pixels.clear();
    nextRow      = 0;
    elapsed      = 0.0;
    textureDirty = false;
}

void RayTracer::destroyTexture() {
    if (previewTexture != 0) {
        glDeleteTextures(1, &previewTexture);
        previewTexture = 0;
    }
}

unsigned int RayTracer::getPreviewTexture() {
    if (pixels.empty()) return 0;

    if (previewTexture == 0) {
        glGenTextures(1, &previewTexture);
        glBindTexture(GL_TEXTURE_2D, previewTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        textureDirty = true;
    }

    if (textureDirty) {
        glBindTexture(GL_TEXTURE_2D, previewTexture);
        // Rows are tightly packed RGB; default alignment of 4 would misread them
        // whenever width*3 is not a multiple of 4.
        GLint previousAlignment = 4;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     settings.width, settings.height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);

        glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureDirty = false;
    }

    return previewTexture;
}
