// Renders an ice block over a patterned floor with an opaque object behind it,
// so the refraction actually has something to bend. Used to tune the material
// numbers quoted to the user - not a unit test.

#include <cstdio>
#include "gl_context.h"
#include <cmath>
#include <vector>

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "SceneLight.h"
#include "Light.h"

class DemoShape : public Shape {
public:
    DemoShape(float x, float y, float z, float scale, int colorIndex, int id)
        : Shape(x, y, z, scale, colorIndex, id) {}
    void draw(GLuint) override {}
};

static DemoShape* makeBox(float x, float y, float z,
                          float sx, float sy, float sz,
                          int colorIndex, int id) {
    DemoShape* s = new DemoShape(x, y, z, 1.0f, colorIndex, id);

    std::vector<glm::vec3> v;
    v.push_back(glm::vec3(-sx, -sy, -sz)); v.push_back(glm::vec3( sx, -sy, -sz));
    v.push_back(glm::vec3( sx,  sy, -sz)); v.push_back(glm::vec3(-sx,  sy, -sz));
    v.push_back(glm::vec3(-sx, -sy,  sz)); v.push_back(glm::vec3( sx, -sy,  sz));
    v.push_back(glm::vec3( sx,  sy,  sz)); v.push_back(glm::vec3(-sx,  sy,  sz));
    s->setVertices(v);

    // Six normals for eight vertices, exactly as Cube does. RayScene sees the
    // count mismatch and falls back to geometric face normals, which is what
    // keeps the faces flat - interpolating corner normals across a box turns
    // every face into a lens and the refraction comes out swirled.
    std::vector<glm::vec3> n;
    n.push_back(glm::vec3( 0,  0,  1)); n.push_back(glm::vec3( 0,  0, -1));
    n.push_back(glm::vec3(-1,  0,  0)); n.push_back(glm::vec3( 1,  0,  0));
    n.push_back(glm::vec3( 0, -1,  0)); n.push_back(glm::vec3( 0,  1,  0));
    s->setNormals(n);

    int quads[6][4] = {
        {0,3,2,1}, {4,5,6,7}, {0,1,5,4}, {3,7,6,2}, {0,4,7,3}, {1,2,6,5}
    };
    std::vector<std::vector<int> > faces;
    for (int f = 0; f < 6; ++f) {
        std::vector<int> q;
        for (int k = 0; k < 4; ++k) q.push_back(quads[f][k]);
        faces.push_back(q);
    }
    s->setFaces(faces);
    return s;
}

static DemoShape* makeSphere(float x, float y, float z, float r,
                             int colorIndex, int id, int segments = 24) {
    DemoShape* s = new DemoShape(x, y, z, r, colorIndex, id);
    std::vector<glm::vec3> verts, norms;
    const float PI = 3.14159265358979f;
    for (int i = 0; i <= segments; ++i) {
        float phi = PI * static_cast<float>(i) / segments;
        for (int j = 0; j <= segments * 2; ++j) {
            float theta = 2.0f * PI * static_cast<float>(j) / (segments * 2);
            glm::vec3 n(std::sin(phi) * std::cos(theta), std::cos(phi),
                        std::sin(phi) * std::sin(theta));
            verts.push_back(n);
            norms.push_back(n);
        }
    }
    s->setVertices(verts);
    s->setNormals(norms);

    const int rowLen = segments * 2 + 1;
    std::vector<std::vector<int> > faces;
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments * 2; ++j) {
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

static DemoShape* makeQuad(const glm::vec3& a, const glm::vec3& b,
                           const glm::vec3& c, const glm::vec3& d,
                           const glm::vec3& normal, int colorIndex, int id) {
    DemoShape* s = new DemoShape(0.0f, 0.0f, 0.0f, 1.0f, colorIndex, id);
    std::vector<glm::vec3> v;
    v.push_back(a); v.push_back(b); v.push_back(c); v.push_back(d);
    s->setVertices(v);
    std::vector<glm::vec3> n(4, normal);
    s->setNormals(n);
    std::vector<std::vector<int> > faces;
    std::vector<int> f; f.push_back(0); f.push_back(1); f.push_back(2); f.push_back(3);
    faces.push_back(f);
    s->setFaces(faces);
    return s;
}

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }
    ShapeManager sm;

    int id = 1;

    // Checkerboard floor: alternating light/dark tiles so the refracted image
    // through the block is obviously displaced relative to the direct view.
    const float tile = 0.7f;
    for (int i = -6; i < 6; ++i) {
        for (int j = -6; j < 6; ++j) {
            int color = ((i + j) & 1) ? 0 : 8;   // dark / light presets
            float x0 = i * tile, z0 = j * tile;
            DemoShape* q = makeQuad(glm::vec3(x0, -1.0f, z0),
                                    glm::vec3(x0 + tile, -1.0f, z0),
                                    glm::vec3(x0 + tile, -1.0f, z0 + tile),
                                    glm::vec3(x0, -1.0f, z0 + tile),
                                    glm::vec3(0.0f, 1.0f, 0.0f), color, id++);
            q->getMaterial().diffuse  = 0.85f;
            q->getMaterial().specular = 0.05f;
            sm.addShape(q);
        }
    }

    // A red sphere sitting behind the block, to be visibly bent by it.
    DemoShape* behind = makeSphere(0.2f, -0.55f, -2.6f, 0.45f, 1, id++);
    behind->getMaterial().diffuse = 0.9f;
    sm.addShape(behind);

    // ---- The ice block -------------------------------------------------
    // Bottom face sits at y = -0.985, NOT -1.0. Resting it exactly on the floor
    // makes the two surfaces coplanar and the tracer speckles along the contact
    // as rays land on one or the other by floating-point luck.
    DemoShape* ice = makeBox(0.0f, -0.135f, 0.3f, 0.85f, 0.85f, 0.85f, 31, id++);
    ice->setCustomColor(0.82f, 0.91f, 1.0f);
    ice->getMaterial().ambient   = 0.25f;
    ice->getMaterial().diffuse   = 0.15f;
    ice->getMaterial().specular  = 1.0f;
    ice->getMaterial().shininess = 180.0f;
    ice->setTransparency(0.88f);
    ice->setIndexOfRefraction(1.31f);
    ice->setReflectivity(0.12f);
    sm.addShape(ice);

    Light* key = new Light(4.0f, 6.0f, 5.0f, id++);
    sm.addShape(key);

    // A second, weaker light from the opposite side. Transparent objects live
    // or die on what is behind them, so a single key light over a dark scene
    // gives a block that reads as smoked glass rather than ice.
    Light* fill = new Light(-5.0f, 3.0f, -4.0f, id++);
    fill->setIntensity(0.5f);
    sm.addShape(fill);

    Camera cam;
    cam.setView(3.14159265f / 2.0f + 0.35f, 0.25f, 6.0f);

    RayTraceSettings s;
    s.mode = RayTraceSettings::Mode_RayTrace;
    s.width = 700;
    s.height = 460;
    s.maxDepth = 8;
    s.samplesPerPixel = 2;
    s.shadows = true;
    s.reflections = true;
    s.refractions = true;
    s.background = glm::vec3(0.55f, 0.68f, 0.85f);

    RayTracer tracer;
    tracer.begin(cam, sm, s);
    std::printf("Tracing %dx%d, %lu triangles...\n", s.width, s.height,
                static_cast<unsigned long>(tracer.getTriangleCount()));

    int guard = 0;
    while (tracer.step(2.0) && guard++ < 100000) {}

    std::printf("Done in %.2f s\n", tracer.elapsedSeconds());
    tracer.saveImage("ice_render.ppm");
    return 0;
}
