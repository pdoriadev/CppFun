// Round-trips the [render] section, and checks the two things about it that
// are easy to get wrong: a file with no [render] must leave settings alone
// rather than stomping them with zeroes, and the values must come back exactly.

#include <cstdio>
#include <fstream>
#include "gl_context.h"

#include "ShapeManager.h"
#include "Camera.h"
#include "SceneIO.h"
#include "RayTracer.h"
#include "Cube.h"
#include "Light.h"

static int failures = 0;

static void check(bool cond, const char* what) {
    if (!cond) { std::printf("  FAIL: %s\n", what); ++failures; }
    else       { std::printf("  ok:   %s\n", what); }
}

static bool nearly(float a, float b, float eps = 1e-4f) {
    float d = a - b;
    return (d < 0 ? -d : d) <= eps;
}

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }

    ShapeManager sm;
    sm.addShape(new Cube(0.0f, 0.0f, 0.0f, 1.0f, 2, 1));
    sm.addShape(new Light(1.0f, 2.0f, 3.0f, 2));
    sm.setShapeCounter(3);

    Camera cam;
    cam.setView(1.1f, 0.4f, 7.5f);

    // Deliberately unlike the defaults on every field.
    RayTraceSettings out;
    out.mode               = RayTraceSettings::Mode_RayCast;
    out.width              = 1280;
    out.height             = 720;
    out.maxDepth           = 9;
    out.samplesPerPixel    = 3;
    out.shadows            = false;
    out.reflections        = false;
    out.refractions        = true;
    out.transparentShadows = false;
    out.shadowBias         = 2.5e-3f;
    out.background         = glm::vec3(0.55f, 0.68f, 0.85f);

    check(SceneIO::save("/tmp/render_section.scene", sm, cam, &out), "saved with [render]");

    // --- comes back intact -------------------------------------------------
    {
        ShapeManager back;
        Camera backCam;
        RayTraceSettings in;                       // starts at defaults
        SceneIO::Result r = SceneIO::load("/tmp/render_section.scene", back, backCam, &in);

        check(r.ok, "load ok");
        check(in.mode == RayTraceSettings::Mode_RayCast, "mode restored");
        check(in.width == 1280 && in.height == 720,      "resolution restored");
        check(in.maxDepth == 9,                          "maxDepth restored");
        check(in.samplesPerPixel == 3,                   "samplesPerPixel restored");
        check(in.shadows == false,                       "shadows restored");
        check(in.reflections == false,                   "reflections restored");
        check(in.refractions == true,                    "refractions restored");
        check(in.transparentShadows == false,            "transparentShadows restored");
        check(nearly(in.shadowBias, 2.5e-3f),            "shadowBias restored");
        check(nearly(in.background.r, 0.55f) &&
              nearly(in.background.g, 0.68f) &&
              nearly(in.background.b, 0.85f),            "background restored");
    }

    // --- a false-valued flag is not mistaken for "absent" -------------------
    // getInt() falls back when the key is missing; "0" must not trip that.
    {
        ShapeManager back;
        Camera backCam;
        RayTraceSettings in;
        in.shadows = true;                          // opposite of what is stored
        SceneIO::load("/tmp/render_section.scene", back, backCam, &in);
        check(in.shadows == false, "stored 0 overrides a true default");
    }

    // --- old file with no [render] leaves settings untouched ----------------
    {
        check(SceneIO::save("/tmp/no_render.scene", sm, cam, 0), "saved without [render]");

        std::ifstream probe("/tmp/no_render.scene");
        std::string text((std::istreambuf_iterator<char>(probe)),
                          std::istreambuf_iterator<char>());
        check(text.find("[render]") == std::string::npos, "no [render] section written");

        ShapeManager back;
        Camera backCam;
        RayTraceSettings in;
        in.maxDepth   = 7;
        in.background = glm::vec3(0.2f, 0.3f, 0.4f);
        SceneIO::load("/tmp/no_render.scene", back, backCam, &in);

        check(in.maxDepth == 7, "maxDepth untouched by a file without [render]");
        check(nearly(in.background.r, 0.2f) && nearly(in.background.b, 0.4f),
              "background untouched by a file without [render]");
    }

    // --- passing null on load must not crash or lose shapes -----------------
    {
        ShapeManager back;
        Camera backCam;
        SceneIO::Result r = SceneIO::load("/tmp/render_section.scene", back, backCam, 0);
        check(r.ok && r.shapesLoaded == 2, "null render pointer still loads shapes");
    }

    std::printf(failures ? "\n%d FAILED\n" : "\nall checks passed\n", failures);
    return failures ? 1 : 0;
}
