// The checkerboard version of the ice-block scene, rebuilt out of real editor
// shapes so it can be saved and reopened.
//
// The original was a throwaway inside a test harness: 144 raw quads on a Shape
// subclass with an empty draw(). That cannot go in a scene file - the loader
// dispatches on serialType(), and "Shape" is not a type it can construct. Each
// tile here is a flattened Cube instead, which both saves and rasterizes.

#include <cstdio>
#include "gl_context.h"

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "SceneIO.h"
#include "Cube.h"
#include "Sphere.h"
#include "Light.h"

static int failures = 0;

static void check(bool cond, const char* what) {
    if (!cond) { std::printf("  FAIL: %s\n", what); ++failures; }
    else       { std::printf("  ok:   %s\n", what); }
}

static bool nearly(float a, float b, float eps = 1e-3f) {
    float d = a - b;
    return (d < 0 ? -d : d) <= eps;
}

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }

    const char* kPath = "scenes/ice-block-checker.scene";
    SceneIO::ensureFolderExists("scenes");

    ShapeManager sm;
    int id = 1;

    // ---------------------------------------------------------- checkerboard
    // 12 x 12 tiles on a 0.7 pitch, spanning -4.2 .. 4.2. Tiles are a hair
    // narrower than the pitch: butting them edge to edge puts coincident faces
    // between neighbours, which speckles the same way a coplanar floor does.
    const float pitch = 0.7f;
    const float tile  = 0.698f;
    const int   half  = 6;

    for (int i = -half; i < half; ++i) {
        for (int j = -half; j < half; ++j) {
            const int   colorIndex = ((i + j) & 1) ? 0 : 8;   // Black / Silver
            const float cx = (static_cast<float>(i) + 0.5f) * pitch;
            const float cz = (static_cast<float>(j) + 0.5f) * pitch;

            Cube* t = new Cube(cx, -1.05f, cz, 1.0f, colorIndex, id++);
            t->useUniformScaling(false);
            t->setScale(tile, 0.1f, tile);       // top face lands at y = -1.0

            char name[32];
            std::snprintf(name, sizeof(name), "Tile %c%d",
                          static_cast<char>('A' + (i + half)), j + half + 1);
            t->setShapeType(name);

            t->getMaterial().ambient  = 0.35f;
            t->getMaterial().diffuse  = 0.85f;
            t->getMaterial().specular = 0.05f;
            sm.addShape(t);
        }
    }

    // Opaque subject behind the block, so the refraction has something to bend.
    Sphere* behind = new Sphere(0.2f, -0.55f, -2.6f, 0.9f, 2, id++);
    behind->setShapeType("Backdrop Sphere");
    behind->getMaterial().ambient   = 0.30f;
    behind->getMaterial().diffuse   = 0.90f;
    behind->getMaterial().specular  = 0.60f;
    behind->getMaterial().shininess = 60.0f;
    sm.addShape(behind);

    // ------------------------------------------------------------- ice block
    // Scale 1.7 on a unit cube gives half-extent 0.85, so the bottom face sits
    // at -0.985: clear of the tiles at -1.0 rather than coplanar with them.
    Cube* ice = new Cube(0.0f, -0.135f, 0.3f, 1.7f, 31, id++);
    ice->setShapeType("Ice Block");
    ice->setCustomColor(0.82f, 0.91f, 1.0f);          // 209, 232, 255
    ice->getMaterial().ambient           = 0.25f;
    ice->getMaterial().diffuse           = 0.15f;
    ice->getMaterial().specular          = 1.0f;
    ice->getMaterial().shininess         = 180.0f;
    ice->getMaterial().reflectivity      = 0.12f;
    ice->getMaterial().transparency      = 0.88f;
    ice->getMaterial().indexOfRefraction = 1.31f;
    sm.addShape(ice);

    Light* key = new Light(4.0f, 6.0f, 5.0f, id++);
    key->setShapeType("Key Light");
    sm.addShape(key);

    Light* fill = new Light(-5.0f, 3.0f, -4.0f, id++);
    fill->setShapeType("Fill Light");
    fill->setIntensity(0.5f);
    sm.addShape(fill);

    sm.setShapeCounter(id);

    Camera cam;
    cam.setView(3.14159265f / 2.0f + 0.35f, 0.25f, 6.0f);

    const int expected = 12 * 12 + 4;
    check(static_cast<int>(sm.getShapes().size()) == expected, "148 shapes built");
    RayTraceSettings s;
    s.mode = RayTraceSettings::Mode_RayTrace;
    s.width = 700;
    s.height = 460;
    s.maxDepth = 8;
    s.samplesPerPixel = 2;
    s.shadows = true;
    s.reflections = true;
    s.refractions = true;
    s.transparentShadows = true;
    s.background = glm::vec3(0.55f, 0.68f, 0.85f);   // 140, 173, 217

    check(SceneIO::save(kPath, sm, cam, &s), "save wrote the file");

    // ------------------------------------------------- reload and verify
    ShapeManager loaded;
    Camera loadedCam;
    RayTraceSettings loadedSettings;              // starts at defaults
    SceneIO::Result r = SceneIO::load(kPath, loaded, loadedCam, &loadedSettings);

    check(r.ok, "load reported success");
    for (size_t i = 0; i < r.warnings.size(); ++i)
        std::printf("  warning: %s\n", r.warnings[i].c_str());
    check(r.shapesLoaded == expected, "every shape came back");
    check(r.shapesSkipped == 0, "nothing was skipped");
    check(nearly(loadedCam.getTheta(), cam.getTheta()), "camera theta restored");
    check(nearly(loadedCam.getPhi(),   cam.getPhi()),   "camera phi restored");

    // Tiles must still alternate, or the board reloaded as a solid slab.
    int blackTiles = 0, silverTiles = 0;
    Shape* reloadedIce = 0;
    std::vector<Shape*>& all = loaded.getShapes();
    for (size_t i = 0; i < all.size(); ++i) {
        if (!all[i]) continue;
        if (all[i]->getShapeType() == "Ice Block") reloadedIce = all[i];
        if (all[i]->getShapeType().compare(0, 5, "Tile ") == 0) {
            if (all[i]->getColorIndex() == 0) ++blackTiles;
            if (all[i]->getColorIndex() == 8) ++silverTiles;
        }
    }
    check(blackTiles == 72 && silverTiles == 72, "72 black and 72 silver tiles");
    check(reloadedIce != 0, "ice block found by name");
    if (reloadedIce) {
        const Material& m = reloadedIce->getMaterial();
        check(nearly(m.transparency, 0.88f),      "transparency 0.88");
        check(nearly(m.indexOfRefraction, 1.31f), "IOR 1.31");
        check(nearly(m.reflectivity, 0.12f),      "reflectivity 0.12");
    }

    // ---------------------------------------------------------------- trace

    RayTracer tracer;
    check(loadedSettings.background == s.background, "background came back in [render]");
    check(loadedSettings.maxDepth == s.maxDepth,     "maxDepth came back in [render]");

    // Trace with the settings read back from the file, not the ones built above.
    tracer.begin(loadedCam, loaded, loadedSettings);
    std::printf("Tracing the reloaded scene: %dx%d, %lu triangles...\n",
                loadedSettings.width, loadedSettings.height,
                static_cast<unsigned long>(tracer.getTriangleCount()));

    int guard = 0;
    while (tracer.step(2.0) && guard++ < 100000) {}
    std::printf("Done in %.2f s\n", tracer.elapsedSeconds());
    check(tracer.saveImage("ice_checker.ppm"), "image written");

    std::printf(failures ? "\n%d FAILED\n" : "\nall checks passed\n", failures);
    return failures ? 1 : 0;
}
