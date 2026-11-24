// Renders a showcase scene exercising shadows, reflection and refraction
// together. Not a unit test - this exists to eyeball the shading.

#include <cstdio>
#include "gl_context.h"
#include <cmath>
#include <vector>

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "SceneLight.h"
#include "Custom.h"
#include "Mobius.h"
#include "Light.h"

class DemoShape : public Shape {
public:
    DemoShape(float x, float y, float z, float scale, int colorIndex, int id)
        : Shape(x, y, z, scale, colorIndex, id) {}
    void draw(GLuint) override {}
};

static DemoShape* makeSphere(float x, float y, float z, float r,
                             int colorIndex, int id, int segments = 24) {
    DemoShape* s = new DemoShape(x, y, z, r, colorIndex, id);

    std::vector<glm::vec3> verts, norms;
    const float PI = 3.14159265358979f;

    for (int i = 0; i <= segments; ++i) {
        float phi = PI * static_cast<float>(i) / segments;      // 0..pi
        for (int j = 0; j <= segments * 2; ++j) {
            float theta = 2.0f * PI * static_cast<float>(j) / (segments * 2);
            glm::vec3 n(std::sin(phi) * std::cos(theta),
                        std::cos(phi),
                        std::sin(phi) * std::sin(theta));
            verts.push_back(n);   // unit sphere; Shape scale handles the radius
            norms.push_back(n);
        }
    }
    s->setVertices(verts);
    s->setNormals(norms);

    const int rowLen = segments * 2 + 1;
    std::vector<std::vector<int> > faces;
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments * 2; ++j) {
            int a = i * rowLen + j;
            int b = a + 1;
            int c = a + rowLen;
            int d = c + 1;

            std::vector<int> t1;
            t1.push_back(a); t1.push_back(c); t1.push_back(b);
            faces.push_back(t1);

            std::vector<int> t2;
            t2.push_back(b); t2.push_back(c); t2.push_back(d);
            faces.push_back(t2);
        }
    }
    s->setFaces(faces);
    return s;
}

static DemoShape* makeFloor(float y, float half, int colorIndex, int id) {
    DemoShape* s = new DemoShape(0.0f, y, 0.0f, 1.0f, colorIndex, id);
    std::vector<glm::vec3> v;
    v.push_back(glm::vec3(-half, 0.0f, -half));
    v.push_back(glm::vec3( half, 0.0f, -half));
    v.push_back(glm::vec3( half, 0.0f,  half));
    v.push_back(glm::vec3(-half, 0.0f,  half));
    s->setVertices(v);

    std::vector<glm::vec3> n(4, glm::vec3(0.0f, 1.0f, 0.0f));
    s->setNormals(n);

    std::vector<std::vector<int> > faces;
    std::vector<int> f;
    f.push_back(0); f.push_back(1); f.push_back(2); f.push_back(3);
    faces.push_back(f);
    s->setFaces(faces);
    return s;
}

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }
    ShapeManager sm;

    DemoShape* floor = makeFloor(-1.0f, 8.0f, 8, 1);
    sm.addShape(floor);

    // Mirror sphere on the left
    DemoShape* mirror = makeSphere(-1.6f, -0.1f, 0.0f, 0.9f, 3, 2);
    mirror->setReflectivity(0.85f);
    sm.addShape(mirror);

    // Glass sphere on the right
    DemoShape* glass = makeSphere(1.6f, -0.1f, 0.4f, 0.9f, 15, 3);
    glass->setTransparency(0.9f);
    glass->setIndexOfRefraction(1.5f);
    sm.addShape(glass);

    // Plain diffuse sphere in the middle and slightly back
    Custom* superellipsoid = new Custom(-0.3f, -0.35f, -1.5f, 0.65f, 2, 4);
    superellipsoid->setReflectivity(0.2f);
    sm.addShape(superellipsoid);

    Mobius* strip = new Mobius(1.4f, 0.55f, -1.2f, 0.85f, 5, 6);
    sm.addShape(strip);

    sm.addShape(new Light(3.0f, 5.0f, 6.0f, 5));

    Camera cam;
    cam.setView(3.14159265f / 2.0f, 0.28f, 6.5f);

    RayTraceSettings s;
    s.width = 640;
    s.height = 400;
    s.maxDepth = 6;
    s.samplesPerPixel = 2;      // 4 rays/pixel
    s.shadows = true;
    s.reflections = true;
    s.refractions = true;
    s.background = glm::vec3(0.05f, 0.06f, 0.10f);

    RayTracer tracer;
    tracer.begin(cam, sm, s);

    std::printf("Tracing %d x %d, %lu triangles...\n",
                s.width, s.height,
                static_cast<unsigned long>(tracer.getTriangleCount()));

    int guard = 0;
    while (tracer.step(2.0) && guard++ < 100000) {}

    std::printf("Done in %.2f s\n", tracer.elapsedSeconds());
    tracer.saveImage("demo_render.ppm");
    return 0;
}
