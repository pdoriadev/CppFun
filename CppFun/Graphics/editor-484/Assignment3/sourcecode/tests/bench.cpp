// Ray tracer throughput benchmark.
//
// "How many FPS does ray tracing get" is the wrong question: the render loop
// gives the tracer a fixed 20 ms slice per frame (Renderer.cpp: step(0.020)),
// so the frame rate while rendering is pinned near 50 regardless of how fast
// the tracer actually is. What changes with your CPU is THROUGHPUT -- how much
// of the image those 20 ms slices get through.
//
// This measures that directly, with no window and no time budget, so the number
// is the tracer's real speed rather than the loop's pacing.
//
// It is CPU-only by design. RayTracer::step() walks scanlines on the host; the
// GPU never participates. Numbers are therefore comparable across machines with
// the same CPU regardless of graphics driver, and a faster GPU changes nothing.

#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

#include "gl_context.h"

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "SceneIO.h"
#include "Cube.h"
#include "Sphere.h"
#include "Light.h"

// Builds the ice-block scene in memory, so the benchmark does not depend on
// scenes/ being present or on any particular working directory.
static void buildScene(ShapeManager& sm) {
    int id = 1;

    Cube* floorSlab = new Cube(0.0f, -1.05f, 0.0f, 1.0f, 30, id++);
    floorSlab->useUniformScaling(false);
    floorSlab->setScale(16.0f, 0.1f, 16.0f);
    floorSlab->getMaterial().ambient  = 0.35f;
    floorSlab->getMaterial().diffuse  = 0.85f;
    floorSlab->getMaterial().specular = 0.05f;
    sm.addShape(floorSlab);

    Sphere* behind = new Sphere(0.2f, -0.55f, -2.6f, 0.9f, 2, id++);
    behind->getMaterial().ambient = 0.30f;
    behind->getMaterial().diffuse = 0.90f;
    sm.addShape(behind);

    Cube* ice = new Cube(0.0f, -0.135f, 0.3f, 1.7f, 31, id++);
    ice->setCustomColor(0.82f, 0.91f, 1.0f);
    ice->getMaterial().ambient           = 0.25f;
    ice->getMaterial().diffuse           = 0.15f;
    ice->getMaterial().specular          = 1.0f;
    ice->getMaterial().shininess         = 180.0f;
    ice->getMaterial().reflectivity      = 0.12f;
    ice->getMaterial().transparency      = 0.88f;
    ice->getMaterial().indexOfRefraction = 1.31f;
    sm.addShape(ice);

    sm.addShape(new Light(4.0f, 6.0f, 5.0f, id++));
    Light* fill = new Light(-5.0f, 3.0f, -4.0f, id++);
    fill->setIntensity(0.5f);
    sm.addShape(fill);

    sm.setShapeCounter(id);
}

struct Case {
    const char* label;
    int  width, height;
    int  spp;          // samples per pixel is spp*spp rays
    int  maxDepth;
    int  mode;
};

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }

    ShapeManager sm;
    buildScene(sm);

    Camera cam;
    cam.setView(3.14159265f / 2.0f + 0.35f, 0.25f, 6.0f);

    const Case cases[] = {
        { "ray cast, 320x240, 1 ray/px",       320, 240, 1, 8,  RayTraceSettings::Mode_RayCast  },
        { "ray trace, 320x240, 1 ray/px",      320, 240, 1, 8,  RayTraceSettings::Mode_RayTrace },
        { "ray trace, 640x480, 1 ray/px",      640, 480, 1, 8,  RayTraceSettings::Mode_RayTrace },
        { "ray trace, 640x480, 4 rays/px",     640, 480, 2, 8,  RayTraceSettings::Mode_RayTrace },
        { "ray trace, 700x460, 4 rays/px, d16", 700, 460, 2, 16, RayTraceSettings::Mode_RayTrace },
    };
    const int nCases = static_cast<int>(sizeof(cases) / sizeof(cases[0]));

    std::printf("Ray tracer throughput  (CPU only -- the GPU takes no part)\n");
    std::printf("%-38s %9s %12s %11s\n", "case", "seconds", "rays/sec", "at 20ms/frame");
    std::printf("%-38s %9s %12s %11s\n", "----", "-------", "--------", "-------------");

    for (int c = 0; c < nCases; ++c) {
        const Case& k = cases[c];

        RayTraceSettings s;
        s.mode              = k.mode;
        s.width             = k.width;
        s.height            = k.height;
        s.samplesPerPixel   = k.spp;
        s.maxDepth          = k.maxDepth;
        s.shadows           = true;
        s.reflections       = true;
        s.refractions       = true;
        s.transparentShadows = true;
        s.background        = glm::vec3(0.55f, 0.68f, 0.85f);

        RayTracer tracer;
        tracer.begin(cam, sm, s);

        // One unbounded step: no 20 ms slicing, so this is the tracer's real
        // speed rather than the render loop's pacing.
        int guard = 0;
        while (tracer.step(1e9) && guard++ < 1000) {}

        const double secs  = tracer.elapsedSeconds();
        const double rays  = double(k.width) * k.height * k.spp * k.spp;
        const double rps   = (secs > 0.0) ? rays / secs : 0.0;
        // How long the same render takes through the editor's render loop,
        // which only hands the tracer 20 ms per frame.
        const double frames = (secs > 0.0) ? secs / 0.020 : 0.0;

        std::printf("%-38s %9.2f %12.0f %8.0f frames\n",
                    k.label, secs, rps, frames);
    }

    std::printf("\nInterpreting this:\n");
    std::printf("  rays/sec is the portable number -- compare it between machines.\n");
    std::printf("  'at 20ms/frame' is how many frames the editor spends on that\n");
    std::printf("  render, since Renderer.cpp gives the tracer step(0.020) per frame.\n");
    std::printf("  Divide by ~50 for wall-clock seconds in the running editor.\n");
    return 0;
}
