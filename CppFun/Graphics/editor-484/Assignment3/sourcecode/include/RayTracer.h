#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Camera.h"
#include "Ray.h"
#include "RayScene.h"

class ShapeManager;

struct RayTraceSettings {
    // Ray casting vs ray tracing, in the classical sense:
    //
    //   Mode_RayCast  - primary rays only. Find the nearest hit, shade it with
    //                   direct lighting, stop. No secondary rays of any kind,
    //                   so no shadows, no reflection, no refraction.
    //   Mode_RayTrace - Whitted-style recursion. Shadow rays plus reflected and
    //                   refracted rays, up to maxDepth bounces.
    //
    // (Appel's 1968 ray casting did trace shadow rays; we exclude them here so
    // the difference between the two modes is unambiguous when demonstrating.)
    enum RenderMode {
        Mode_RayCast  = 0,
        Mode_RayTrace = 1
    };

    int   mode;
    int   width;
    int   height;
    int   maxDepth;           // recursion limit for reflection/refraction
    int   samplesPerPixel;    // 1 = no antialiasing; N*N grid otherwise
    bool  shadows;
    bool  reflections;
    bool  refractions;

    // When true a shadow ray is attenuated by the transparency of whatever it
    // crosses instead of being stopped outright, so glass and ice cast pale
    // shadows and the surfaces behind them keep their highlights. Turning it
    // off restores the textbook binary shadow test, which is worth showing
    // side by side.
    bool  transparentShadows;
    glm::vec3 background;
    float shadowBias;         // nudge along the normal to avoid self-shadowing

    RayTraceSettings()
        : mode(Mode_RayTrace),
          width(640), height(480), maxDepth(4), samplesPerPixel(1),
          shadows(true), reflections(true), refractions(true),
          transparentShadows(true),
          background(0.1f, 0.1f, 0.1f), shadowBias(1e-3f) {}
};

// Incremental CPU ray tracer.
//
// Rendering is driven a slice at a time from the main loop rather than run to
// completion in one call. That keeps the UI responsive and gives a progressive
// preview for free, without threads - which matters because MinGW toolchains
// built with the win32 threads model have no working std::thread.
class RayTracer {
public:
    RayTracer();
    ~RayTracer();

    // Snapshots the scene and camera, then begins a render.
    void begin(const Camera& camera, ShapeManager& shapeManager,
               const RayTraceSettings& settings);

    // Renders for up to `budgetSeconds`. Returns true while work remains.
    bool step(double budgetSeconds);

    void cancel();

    bool  isRendering() const { return rendering; }
    bool  hasImage() const { return !pixels.empty(); }
    float progress() const;              // 0..1
    double elapsedSeconds() const { return elapsed; }

    int   getWidth() const { return settings.width; }
    int   getHeight() const { return settings.height; }
    size_t getTriangleCount() const { return scene.triangleCount(); }

    const std::vector<unsigned char>& getPixels() const { return pixels; }

    bool saveImage(const std::string& path) const;

    // Lazily created GL texture holding the current (possibly partial) image.
    // Returns 0 if there is nothing to show.
    unsigned int getPreviewTexture();

    // Throws away the image and its GL texture. Called when a scene is replaced:
    // a preview of the previous scene left on screen is worse than a blank panel.
    // Must be called with a current GL context.
    void discardImage();

private:
    RayTraceSettings settings;
    RayScene   scene;
    Camera     camera;

    std::vector<unsigned char> pixels;   // RGB8, row 0 = top
    int    nextRow;
    bool   rendering;
    double elapsed;

    unsigned int previewTexture;
    bool   textureDirty;

    glm::vec3 traceRay(const Ray& ray, int depth) const;
    glm::vec3 shade(const Hit& hit, const Ray& ray, int depth) const;

    void destroyTexture();
};

#endif // RAYTRACER_H
