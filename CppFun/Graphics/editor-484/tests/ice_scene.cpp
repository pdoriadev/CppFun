// Builds the ice-block demo out of real editor shapes, saves it through
// SceneIO, loads it straight back into a fresh ShapeManager, and ray traces
// the *loaded* copy.
//
// Tracing the reloaded scene rather than the one just built is the point: it
// proves the file carries everything the render needs, camera angle included.

#include <cstdio>
#include "gl_context.h"

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "RayTracer.h"
#include "SceneIO.h"
#include "Settings.h"
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

    const char* kPath = "scenes/ice-block.scene";
    SceneIO::ensureFolderExists("scenes");

    // ---------------------------------------------------------------- build
    ShapeManager sm;
    int id = 1;

    // Floor: a cube flattened into a slab. Its top face lands at y = -1.0
    // (centre -1.05, half-height 0.05 after the 0.1 y scale).
    Cube* floorSlab = new Cube(0.0f, -1.05f, 0.0f, 1.0f, 30, id++);   // Light Gray
    floorSlab->useUniformScaling(false);
    floorSlab->setScale(16.0f, 0.1f, 16.0f);
    floorSlab->setShapeType("Floor");
    floorSlab->getMaterial().ambient  = 0.35f;
    floorSlab->getMaterial().diffuse  = 0.85f;
    floorSlab->getMaterial().specular = 0.05f;
    sm.addShape(floorSlab);

    // Something opaque behind the block, so the refraction has a subject.
    Sphere* behind = new Sphere(0.2f, -0.55f, -2.6f, 0.9f, 2, id++);  // Red
    behind->setShapeType("Backdrop Sphere");
    behind->getMaterial().ambient = 0.3f;
    behind->getMaterial().diffuse = 0.9f;
    sm.addShape(behind);

    // ---- the ice block ----
    // Cube spans -0.5..0.5, so scale 1.7 gives half-extent 0.85 and the bottom
    // face sits at -0.135 - 0.85 = -0.985: 15 thousandths clear of the floor.
    // Resting it exactly on y = -1.0 makes the two surfaces coplanar and the
    // contact line speckles.
    Cube* ice = new Cube(0.0f, -0.135f, 0.3f, 1.7f, 31, id++);        // Custom
    ice->setShapeType("Ice Block");
    ice->setCustomColor(0.82f, 0.91f, 1.0f);
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

    // Fill from behind and to the left. A transparent object is mostly whatever
    // is behind it, so lighting only the camera side leaves the block dark.
    Light* fill = new Light(-5.0f, 3.0f, -4.0f, id++);
    fill->setShapeType("Fill Light");
    fill->setIntensity(0.5f);
    sm.addShape(fill);

    sm.setShapeCounter(id);

    Camera cam;
    cam.setView(3.14159265f / 2.0f + 0.35f, 0.25f, 6.0f);

    // ----------------------------------------------------------------- save
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

    check(SceneIO::save(kPath, sm, cam, &s), "save wrote the file");

    // -------------------------------------------------- load into a new one
    ShapeManager loaded;
    Camera loadedCam;                       // deliberately left at defaults
    RayTraceSettings loadedSettings;              // starts at defaults
    SceneIO::Result r = SceneIO::load(kPath, loaded, loadedCam, &loadedSettings);

    check(r.ok, "load reported success");
    for (size_t i = 0; i < r.warnings.size(); ++i)
        std::printf("  warning: %s\n", r.warnings[i].c_str());
    check(r.shapesLoaded == 5, "five shapes came back");
    check(r.shapesSkipped == 0, "nothing was skipped");

    check(nearly(loadedCam.getTheta(), cam.getTheta()), "camera theta restored");
    check(nearly(loadedCam.getPhi(),   cam.getPhi()),   "camera phi restored");
    check(nearly(loadedCam.getRadius(), 6.0f),          "camera radius restored");
    check(nearly(loadedCam.getPosition().x, cam.getPosition().x) &&
          nearly(loadedCam.getPosition().y, cam.getPosition().y) &&
          nearly(loadedCam.getPosition().z, cam.getPosition().z),
          "camera position matches the original");

    // Find the ice block again and confirm every material field survived.
    Shape* reloadedIce = 0;
    std::vector<Shape*>& all = loaded.getShapes();
    for (size_t i = 0; i < all.size(); ++i)
        if (all[i] && all[i]->getShapeType() == "Ice Block") reloadedIce = all[i];

    check(reloadedIce != 0, "ice block found by name");
    if (reloadedIce) {
        const Material& m = reloadedIce->getMaterial();
        check(nearly(m.transparency, 0.88f),      "transparency 0.88");
        check(nearly(m.indexOfRefraction, 1.31f), "IOR 1.31");
        check(nearly(m.reflectivity, 0.12f),      "reflectivity 0.12");
        check(nearly(m.shininess, 180.0f),        "shininess 180");
        check(nearly(m.diffuse, 0.15f),           "diffuse 0.15");
        check(nearly(m.specular, 1.0f),           "specular 1.0");
        check(nearly(m.ambient, 0.25f),           "ambient 0.25");
        const float* cc = reloadedIce->getCustomColor();
        check(nearly(cc[0], 0.82f) && nearly(cc[1], 0.91f) && nearly(cc[2], 1.0f),
              "custom colour 0.82 0.91 1.0");
        check(reloadedIce->getColorIndex() == 31, "colour index is Custom");
    }

    // Floor kept its non-uniform scale?
    Shape* reloadedFloor = 0;
    for (size_t i = 0; i < all.size(); ++i)
        if (all[i] && all[i]->getShapeType() == "Floor") reloadedFloor = all[i];
    check(reloadedFloor != 0, "floor found by name");
    if (reloadedFloor) {
        check(!reloadedFloor->isUsingUniformScaling(), "floor still non-uniform");
        glm::vec3 s = reloadedFloor->getNonUniformScale();
        check(nearly(s.x, 16.0f) && nearly(s.y, 0.1f) && nearly(s.z, 16.0f),
              "floor scale 16 x 0.1 x 16");
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
    check(tracer.saveImage("ice_scene.ppm"), "image written");

    std::printf(failures ? "\n%d FAILED\n" : "\nall checks passed\n", failures);
    return failures ? 1 : 0;
}
