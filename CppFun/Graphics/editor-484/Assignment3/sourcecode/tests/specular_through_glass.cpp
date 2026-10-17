// Why does a shiny object lose its highlight when seen through a transparent
// one? Measures the peak brightness on a glossy sphere under four conditions
// so the cause is isolated rather than guessed at.

#include <cstdio>
#include "gl_context.h"
#include <cmath>
#include <vector>

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Cube.h"
#include "Sphere.h"
#include "Light.h"

// Peak luminance over the whole frame, ignoring the background colour.
static float peakLuma(const std::vector<unsigned char>& px, int w, int h,
                      float bgLuma) {
    float best = 0.0f;
    for (int i = 0; i + 2 < static_cast<int>(px.size()); i += 3) {
        float l = 0.2126f * px[i] + 0.7152f * px[i + 1] + 0.0722f * px[i + 2];
        l /= 255.0f;
        if (l > bgLuma + 0.02f && l > best) best = l;
    }
    (void)w; (void)h;
    return best;
}

struct Config {
    const char* label;
    bool  includeBlock;
    float transparency;
    bool  shadows;
};

static float run(const Config& c) {
    ShapeManager sm;
    int id = 1;

    // Glossy sphere. High ks and a tight exponent give an unmistakable highlight.
    Sphere* ball = new Sphere(0.0f, 0.0f, -1.6f, 1.4f, 2, id++);
    ball->getMaterial().ambient   = 0.10f;
    ball->getMaterial().diffuse   = 0.45f;
    ball->getMaterial().specular  = 1.0f;
    ball->getMaterial().shininess = 90.0f;
    sm.addShape(ball);

    if (c.includeBlock) {
        // A slab standing between the camera and the ball.
        Cube* slab = new Cube(0.0f, 0.0f, 0.9f, 1.0f, 31, id++);
        slab->useUniformScaling(false);
        slab->setScale(4.0f, 4.0f, 0.35f);
        slab->setCustomColor(0.82f, 0.91f, 1.0f);
        slab->getMaterial().ambient           = 0.25f;
        slab->getMaterial().diffuse           = 0.15f;
        slab->getMaterial().specular          = 1.0f;
        slab->getMaterial().shininess         = 180.0f;
        slab->getMaterial().transparency      = c.transparency;
        slab->getMaterial().indexOfRefraction = 1.31f;
        sm.addShape(slab);
    }

    // Key light on the camera side, high and to the right: its highlight on the
    // ball is visible from the camera, and the slab sits in the light path.
    sm.addShape(new Light(3.0f, 3.0f, 4.0f, id++));

    Camera cam;
    cam.setView(3.14159265f / 2.0f, 0.0f, 5.0f);

    RayTraceSettings s;
    s.mode = RayTraceSettings::Mode_RayTrace;
    s.width = 240;
    s.height = 180;
    s.maxDepth = 8;
    s.samplesPerPixel = 1;
    s.shadows = c.shadows;
    s.reflections = true;
    s.refractions = true;
    s.background = glm::vec3(0.0f, 0.0f, 0.0f);

    RayTracer tracer;
    tracer.begin(cam, sm, s);
    int guard = 0;
    while (tracer.step(2.0) && guard++ < 100000) {}

    return peakLuma(tracer.getPixels(), s.width, s.height, 0.0f);
}

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }

    Config configs[] = {
        { "ball alone (baseline)",                false, 0.00f, true  },
        { "ball behind OPAQUE block",             true,  0.00f, true  },
        { "ball behind TRANSPARENT block",        true,  0.88f, true  },
        { "ball behind TRANSPARENT block, shadows OFF", true, 0.88f, false },
    };

    float base = 0.0f;
    for (int i = 0; i < 4; ++i) {
        float p = run(configs[i]);
        if (i == 0) base = p;
        std::printf("%-46s peak luma %.4f   (%.0f%% of baseline)\n",
                    configs[i].label, p,
                    base > 0.0f ? 100.0f * p / base : 0.0f);
    }
    return 0;
}
