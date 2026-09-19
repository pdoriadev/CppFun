// Renders a textured scene for visual inspection. Not an assertion suite --
// this one is judged by eye, like ice_scene and demo_render.
//
// Writes texture_scene.ppm plus the checkerboard it generates, so it needs no
// fixture in the repo.

#include <cstdio>
#include <cmath>
#include <vector>

#include "gl_context.h"

#include "ShapeManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "ImageIO.h"
#include "Cube.h"
#include "Sphere.h"
#include "Light.h"

// 256x256 checkerboard, warm and cool squares so the projection is obvious on
// every face of the cube and around the sphere.
static bool writeChecker(const char* path) {
    const int n = 256, square = 32;
    std::vector<unsigned char> px(static_cast<size_t>(n) * n * 3);
    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            const bool on = ((x / square) + (y / square)) % 2 == 0;
            const size_t o = (static_cast<size_t>(y) * n + x) * 3;
            px[o + 0] = on ? 230 : 40;
            px[o + 1] = on ? 180 : 70;
            px[o + 2] = on ? 90  : 150;
        }
    }
    return ImageIO::writeBMP(path, px, n, n);
}

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }

    const char* checker = "texture_scene_checker.bmp";
    if (!writeChecker(checker)) { std::printf("could not write the checker\n"); return 1; }

    ShapeManager sm;
    int id = 1;

    Cube* floorSlab = new Cube(0.0f, -1.2f, 0.0f, 1.0f, 30, id++);
    floorSlab->useUniformScaling(false);
    floorSlab->setScale(14.0f, 0.1f, 14.0f);
    floorSlab->getMaterial().texturePath = checker;
    floorSlab->getMaterial().ambient = 0.35f;
    floorSlab->getMaterial().diffuse = 0.9f;
    floorSlab->getMaterial().specular = 0.05f;
    sm.addShape(floorSlab);

    Sphere* ball = new Sphere(-1.15f, -0.35f, 0.0f, 1.6f, 31, id++);
    ball->setCustomColor(1.0f, 1.0f, 1.0f);
    ball->getMaterial().texturePath = checker;
    ball->getMaterial().ambient  = 0.25f;
    ball->getMaterial().diffuse  = 0.9f;
    ball->getMaterial().specular = 0.4f;
    sm.addShape(ball);

    Cube* box = new Cube(1.25f, -0.35f, 0.2f, 1.5f, 31, id++);
    box->setCustomColor(1.0f, 1.0f, 1.0f);
    box->setRotation(0.0f, 28.0f, 0.0f);
    box->getMaterial().texturePath = checker;
    box->getMaterial().ambient  = 0.25f;
    box->getMaterial().diffuse  = 0.9f;
    box->getMaterial().specular = 0.35f;
    sm.addShape(box);

    // One untextured sphere, so the render also shows that an untextured
    // surface still looks exactly as it always did.
    Sphere* plain = new Sphere(0.05f, -0.75f, 1.9f, 0.8f, 9, id++);
    plain->getMaterial().ambient = 0.3f;
    plain->getMaterial().diffuse = 0.9f;
    sm.addShape(plain);

    sm.addShape(new Light(4.0f, 6.0f, 5.0f, id++));
    Light* fill = new Light(-5.0f, 3.0f, -3.0f, id++);
    fill->setIntensity(0.45f);
    sm.addShape(fill);
    sm.setShapeCounter(id);

    Camera cam;
    cam.setView(3.14159265f / 2.0f + 0.42f, 0.30f, 6.4f);

    RayTraceSettings s;
    s.mode              = RayTraceSettings::Mode_RayTrace;
    s.width             = 700;
    s.height            = 460;
    s.samplesPerPixel   = 2;          // 4 rays per pixel
    s.maxDepth          = 6;
    s.shadows           = true;
    s.reflections       = true;
    s.refractions       = true;
    s.transparentShadows = true;
    s.background        = glm::vec3(0.55f, 0.68f, 0.85f);

    RayTracer tracer;
    tracer.begin(cam, sm, s);
    int guard = 0;
    while (tracer.step(1e9) && guard++ < 1000) {}

    if (!tracer.saveImage("texture_scene.ppm")) {
        std::printf("could not write texture_scene.ppm\n");
        return 1;
    }
    std::printf("wrote texture_scene.ppm (%.2f s)\n", tracer.elapsedSeconds());
    return 0;
}
