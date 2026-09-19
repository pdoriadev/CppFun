#include <cstdio>
#include "gl_context.h"
#include <cmath>
#include "ShapeManager.h"
#include "Shape.h"
#include "Camera.h"
#include "SceneIO.h"
#include "Light.h"
#include "Cube.h"
#include "SimpleCloth.h"

static int failures = 0;
static void check(bool c, const char* what) {
    std::printf("  %s  %s\n", c ? "PASS" : "FAIL", what);
    if (!c) ++failures;
}

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }
    std::printf("Scene save/load round trip\n\n");

    ShapeManager a;
    Cube* cube = new Cube(1.0f, 2.0f, 3.0f, 1.5f, 7, a.incrementShapeCounter());
    cube->setRotation(10.0f, 20.0f, 30.0f);
    Material m;
    m.ambient = 0.4f; m.diffuse = 0.6f; m.specular = 0.2f; m.shininess = 90.0f;
    m.reflectivity = 0.7f; m.transparency = 0.3f; m.indexOfRefraction = 1.7f;
    cube->setMaterial(m);
    a.addShape(cube);

    Light* light = new Light(-2.0f, 4.0f, 1.0f, a.incrementShapeCounter());
    light->setType(Light::Directional);
    light->setColor(glm::vec3(0.2f, 0.4f, 0.9f));
    light->setIntensity(2.5f);
    light->setDirection(glm::vec3(0.0f, -1.0f, -0.5f));
    light->setLinearAttenuation(0.25f);
    light->setEnabled(false);
    a.addShape(light);

    a.addShape(new SimpleCloth(0,0,0, 1.0f, 6, a.incrementShapeCounter(), 2.0f, 0.1f, 9));

    Camera camA;
    camA.setView(1.1f, 0.5f, 8.5f, glm::vec3(1.0f, 2.0f, 3.0f));

    check(SceneIO::save("/tmp/roundtrip.scene", a, camA), "scene saves");

    ShapeManager b;
    Camera camB;
    SceneIO::Result r = SceneIO::load("/tmp/roundtrip.scene", b, camB);
    check(r.ok, "scene loads");
    check(r.shapesLoaded == 3, "all three objects restored");
    check(b.getShapes().size() == 3, "shape manager holds three objects");

    Cube* cube2 = dynamic_cast<Cube*>(b.getShapes()[0]);
    check(cube2 != 0, "cube type restored");
    if (cube2) {
        check(std::fabs(cube2->getX() - 1.0f) < 1e-4f &&
              std::fabs(cube2->getY() - 2.0f) < 1e-4f &&
              std::fabs(cube2->getZ() - 3.0f) < 1e-4f, "position restored");
        check(std::fabs(cube2->getAngleY() - 20.0f) < 1e-3f, "rotation restored");
        check(std::fabs(cube2->getScale() - 1.5f) < 1e-4f, "scale restored");
        check(cube2->getColorIndex() == 7, "colour index restored");
        const Material& m2 = cube2->getMaterial();
        check(std::fabs(m2.shininess - 90.0f) < 1e-3f &&
              std::fabs(m2.reflectivity - 0.7f) < 1e-4f &&
              std::fabs(m2.transparency - 0.3f) < 1e-4f &&
              std::fabs(m2.indexOfRefraction - 1.7f) < 1e-4f, "material restored");
    }

    Light* light2 = dynamic_cast<Light*>(b.getShapes()[1]);
    check(light2 != 0, "light type restored");
    if (light2) {
        check(light2->getType() == Light::Directional, "light type (directional) restored");
        check(!light2->isEnabled(), "disabled flag restored");
        check(std::fabs(light2->getIntensity() - 2.5f) < 1e-4f, "intensity restored");
        check(std::fabs(light2->getColor().b - 0.9f) < 1e-4f, "light colour restored");
        check(std::fabs(light2->getLinearAttenuation() - 0.25f) < 1e-4f, "attenuation restored");
    }

    SimpleCloth* cloth2 = dynamic_cast<SimpleCloth*>(b.getShapes()[2]);
    check(cloth2 != 0, "cloth type restored");
    if (cloth2) check(cloth2->getClothSize() == 9, "cloth grid size restored");

    check(std::fabs(camB.getRadius() - 8.5f) < 1e-3f, "camera radius restored");
    check(glm::length(camB.getTarget() - glm::vec3(1,2,3)) < 1e-3f, "camera target restored");
    check(b.getShapeCounter() >= 3, "shape counter kept ahead of used ids");

    // A corrupt file must not destroy the open scene.
    ShapeManager c;
    c.addShape(new Cube(0,0,0,1,2, c.incrementShapeCounter()));
    Camera camC;
    SceneIO::Result bad = SceneIO::load("/tmp/definitely_not_here.scene", c, camC);
    check(!bad.ok, "missing file reports failure");
    check(c.getShapes().size() == 1, "failed load leaves the current scene intact");

    std::printf("\n%s (%d failure%s)\n", failures ? "TESTS FAILED" : "ALL TESTS PASSED",
                failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
