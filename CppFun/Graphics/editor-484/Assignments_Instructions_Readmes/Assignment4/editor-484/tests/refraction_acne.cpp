// Reproduces the speckle seen on a smooth-shaded transparent mesh and
// isolates its cause.
//
// The test subject is a bumpy torus: smooth interpolated vertex normals, deep
// concavities, and up to six surface crossings along a single primary ray -
// the same conditions an imported gargoyle presents, without needing the mesh.
//
// Artifact metric: for every pixel, the distance from the median of its 3x3
// neighbourhood. Genuine refraction detail is locally coherent; salt-and-pepper
// noise is not, so a rising count means more noise, not more detail.

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

#include "gl_context.h"

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Cube.h"
#include "Sphere.h"
#include "Light.h"

class MeshShape : public Shape {
public:
    MeshShape(float x, float y, float z, float scale, int colorIndex, int id)
        : Shape(x, y, z, scale, colorIndex, id) {}
    void draw(GLuint) override {}
};

static const float PI = 3.14159265358979f;

// Torus with a sinusoidal displacement on the tube radius. Vertex normals come
// from central differences of the same parametrisation, so they are smooth and
// disagree with the per-triangle face normals exactly the way an OBJ's do.
static glm::vec3 bumpyTorus(float u, float v) {
    const float R = 1.0f;
    float r = 0.42f + 0.09f * std::sin(5.0f * u) * std::sin(4.0f * v);
    float cx = (R + r * std::cos(v)) * std::cos(u);
    float cy = r * std::sin(v);
    float cz = (R + r * std::cos(v)) * std::sin(u);
    return glm::vec3(cx, cy, cz);
}

static MeshShape* makeBumpyTorus(int id, int nu = 160, int nv = 80) {
    MeshShape* s = new MeshShape(0.0f, 0.1f, 0.0f, 1.0f, 31, id);

    std::vector<glm::vec3> verts, norms;
    const float h = 1e-3f;

    for (int i = 0; i <= nu; ++i) {
        float u = 2.0f * PI * static_cast<float>(i) / nu;
        for (int j = 0; j <= nv; ++j) {
            float v = 2.0f * PI * static_cast<float>(j) / nv;
            verts.push_back(bumpyTorus(u, v));

            glm::vec3 du = bumpyTorus(u + h, v) - bumpyTorus(u - h, v);
            glm::vec3 dv = bumpyTorus(u, v + h) - bumpyTorus(u, v - h);
            glm::vec3 n  = glm::cross(dv, du);
            float len = glm::length(n);
            norms.push_back(len > 1e-9f ? n / len : glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }
    s->setVertices(verts);
    s->setNormals(norms);

    const int rowLen = nv + 1;
    std::vector<std::vector<int> > faces;
    for (int i = 0; i < nu; ++i) {
        for (int j = 0; j < nv; ++j) {
            int a = i * rowLen + j, b = a + 1, c = a + rowLen, d = c + 1;
            std::vector<int> t1; t1.push_back(a); t1.push_back(c); t1.push_back(b);
            faces.push_back(t1);
            std::vector<int> t2; t2.push_back(b); t2.push_back(c); t2.push_back(d);
            faces.push_back(t2);
        }
    }
    s->setFaces(faces);
    return s;
}

struct Frame {
    std::vector<unsigned char> px;
    int w, h;
};

static Frame render(int maxDepth) {
    ShapeManager sm;
    int id = 1;

    Cube* floorSlab = new Cube(0.0f, -1.55f, 0.0f, 1.0f, 30, id++);
    floorSlab->useUniformScaling(false);
    floorSlab->setScale(16.0f, 0.1f, 16.0f);
    floorSlab->getMaterial().ambient  = 0.35f;
    floorSlab->getMaterial().diffuse  = 0.85f;
    floorSlab->getMaterial().specular = 0.05f;
    sm.addShape(floorSlab);

    // Bright saturated ball behind, so any stray ray that reaches it shows up
    // as an unmistakable coloured dot on the glass.
    Sphere* ball = new Sphere(-1.1f, 0.0f, -2.2f, 1.8f, 16, id++);   // Orange
    ball->getMaterial().ambient   = 0.30f;
    ball->getMaterial().diffuse   = 0.90f;
    ball->getMaterial().specular  = 0.80f;
    ball->getMaterial().shininess = 90.0f;
    sm.addShape(ball);

    MeshShape* glassy = makeBumpyTorus(id++);
    glassy->setCustomColor(0.82f, 0.91f, 1.0f);
    glassy->getMaterial().ambient           = 0.25f;
    glassy->getMaterial().diffuse           = 0.15f;
    glassy->getMaterial().specular          = 1.0f;
    glassy->getMaterial().shininess         = 180.0f;
    glassy->getMaterial().reflectivity      = 0.12f;
    glassy->getMaterial().transparency      = 0.88f;
    glassy->getMaterial().indexOfRefraction = 1.31f;
    sm.addShape(glassy);

    sm.addShape(new Light(4.0f, 6.0f, 5.0f, id++));

    Camera cam;
    cam.setView(PI / 2.0f + 0.3f, 0.35f, 5.0f);

    RayTraceSettings s;
    s.mode = RayTraceSettings::Mode_RayTrace;
    s.width = 320;
    s.height = 240;
    s.maxDepth = maxDepth;
    s.samplesPerPixel = 1;          // no AA: it would hide the speckle
    s.shadows = true;
    s.reflections = true;
    s.refractions = true;
    s.background = glm::vec3(0.55f, 0.68f, 0.85f);

    RayTracer tracer;
    tracer.begin(cam, sm, s);
    int guard = 0;
    while (tracer.step(2.0) && guard++ < 100000) {}

    Frame f;
    f.px = tracer.getPixels();
    f.w = s.width;
    f.h = s.height;
    return f;
}

// Count pixels that differ sharply from the median of their neighbours.
static int speckleCount(const Frame& f, int threshold = 60) {
    int count = 0;
    for (int y = 1; y < f.h - 1; ++y) {
        for (int x = 1; x < f.w - 1; ++x) {
            int worst = 0;
            for (int c = 0; c < 3; ++c) {
                int vals[9], n = 0;
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx)
                        vals[n++] = f.px[((y + dy) * f.w + (x + dx)) * 3 + c];
                int self = f.px[(y * f.w + x) * 3 + c];
                std::sort(vals, vals + 9);
                int d = self - vals[4];
                if (d < 0) d = -d;
                if (d > worst) worst = d;
            }
            if (worst > threshold) ++count;
        }
    }
    return count;
}

static bool writePpm(const char* path, const Frame& f) {
    FILE* fp = std::fopen(path, "wb");
    if (!fp) return false;
    std::fprintf(fp, "P6\n%d %d\n255\n", f.w, f.h);
    std::fwrite(&f.px[0], 1, f.px.size(), fp);
    std::fclose(fp);
    return true;
}

int main(int argc, char** argv) {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }

    const char* tag = (argc > 1) ? argv[1] : "run";

    const int depths[] = { 4, 8, 16 };
    for (int i = 0; i < 3; ++i) {
        Frame f = render(depths[i]);
        int speckle = speckleCount(f);
        std::printf("%-10s maxDepth %2d  speckled pixels %5d  (%.2f%% of frame)\n",
                    tag, depths[i], speckle,
                    100.0f * speckle / (f.w * f.h));

        char name[128];
        std::snprintf(name, sizeof(name), "acne_%s_d%d.ppm", tag, depths[i]);
        writePpm(name, f);
    }
    return 0;
}
