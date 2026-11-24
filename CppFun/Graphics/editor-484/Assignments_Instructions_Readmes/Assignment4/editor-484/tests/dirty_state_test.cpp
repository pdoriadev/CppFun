// The "it says I have unsaved changes and I don't" family of bugs.
//
// The dirty flag is a content hash compared against a baseline. Two separate
// things can break it, and both look identical to the user:
//
//   1. The baseline is never established (uninitialised savedSignature).
//   2. The baseline is fine but save/load is lossy, so a file reopened right
//      after saving hashes differently than what was written.
//
// (1) lives in Renderer and needs a window. (2) is pure SceneIO and is what
// this file pins down: a scene must reload to a hash-identical scene, for
// awkward float values as well as tidy ones.

#include <cstdio>
#include <cmath>
#include "gl_context.h"

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "SceneIO.h"
#include "Cube.h"
#include "Sphere.h"
#include "Light.h"

static int failures = 0;

static void check(bool cond, const char* what) {
    if (!cond) { std::printf("  FAIL: %s\n", what); ++failures; }
    else       { std::printf("  ok:   %s\n", what); }
}

// Same FNV-1a over the same fields as Renderer::sceneSignature. Kept in step
// deliberately: this test is only meaningful if it hashes what that hashes.
struct Mix {
    unsigned long long h;
    Mix() : h(1469598103934665603ULL) {}
    void bytes(const void* p, size_t n) {
        const unsigned char* b = static_cast<const unsigned char*>(p);
        for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ULL; }
    }
    void f(float v) { bytes(&v, sizeof(v)); }
    void i(int v)   { bytes(&v, sizeof(v)); }
    void s(const std::string& v) { bytes(v.data(), v.size()); i(0); }
};

static unsigned long long signature(ShapeManager& sm) {
    Mix mix;
    std::vector<Shape*>& shapes = sm.getShapes();
    mix.i(static_cast<int>(shapes.size()));
    for (size_t k = 0; k < shapes.size(); ++k) {
        Shape* sh = shapes[k];
        if (!sh) continue;
        mix.s(sh->serialType());
        mix.s(sh->getShapeType());
        mix.s(sh->getSourcePath());
        mix.i(sh->getId());
        mix.f(sh->getX()); mix.f(sh->getY()); mix.f(sh->getZ());
        mix.f(sh->getAngleX()); mix.f(sh->getAngleY()); mix.f(sh->getAngleZ());
        mix.i(sh->isUsingUniformScaling() ? 1 : 0);
        mix.f(sh->getScale());
        glm::vec3 nu = sh->getNonUniformScale();
        mix.f(nu.x); mix.f(nu.y); mix.f(nu.z);
        mix.i(sh->getColorIndex());
        const float* cc = sh->getCustomColor();
        mix.f(cc[0]); mix.f(cc[1]); mix.f(cc[2]);
        const Material& m = sh->getMaterial();
        mix.f(m.ambient); mix.f(m.diffuse); mix.f(m.specular); mix.f(m.shininess);
        mix.f(m.reflectivity); mix.f(m.transparency); mix.f(m.indexOfRefraction);
        if (Light* L = dynamic_cast<Light*>(sh)) {
            mix.i(L->getType());
            mix.i(L->isEnabled() ? 1 : 0);
            mix.f(L->getColor().r); mix.f(L->getColor().g); mix.f(L->getColor().b);
            mix.f(L->getIntensity());
            mix.f(L->getAmbientScale()); mix.f(L->getDiffuseScale()); mix.f(L->getSpecularScale());
            mix.f(L->getDirection().x); mix.f(L->getDirection().y); mix.f(L->getDirection().z);
            mix.f(L->getConstantAttenuation());
            mix.f(L->getLinearAttenuation());
            mix.f(L->getQuadraticAttenuation());
        }
    }
    return mix.h;
}

int main() {
    if (!makeHeadlessContext()) { std::printf("no GL context\n"); return 77; }

    // --- tidy values, the easy case ----------------------------------------
    {
        ShapeManager a;
        a.addShape(new Cube(1.0f, -0.135f, 0.3f, 1.7f, 31, 1));
        a.getShapes()[0]->setCustomColor(0.82f, 0.91f, 1.0f);
        a.getShapes()[0]->getMaterial().transparency = 0.88f;
        a.addShape(new Light(4.0f, 6.0f, 5.0f, 2));
        a.setShapeCounter(3);

        Camera cam;
        SceneIO::save("/tmp/dirty_tidy.scene", a, cam);

        ShapeManager b;
        Camera cam2;
        SceneIO::load("/tmp/dirty_tidy.scene", b, cam2);

        check(signature(a) == signature(b), "tidy values round-trip to the same hash");
    }

    // --- awkward values ----------------------------------------------------
    // A shape dragged with the mouse, or scaled by a computation, lands on
    // values with a full float's worth of digits. If the writer truncates, the
    // reloaded scene hashes differently and the editor claims unsaved changes
    // the instant you reopen the file you just wrote.
    {
        ShapeManager a;
        Sphere* s = new Sphere(1.0f / 3.0f, std::sqrt(2.0f), -1.0f / 7.0f,
                               0.123456789f, 31, 1);
        s->setRotation(1.0f / 3.0f, 45.4545455f, -0.000123456f);
        s->setCustomColor(1.0f / 3.0f, 2.0f / 7.0f, 0.9999999f);
        s->getMaterial().ambient           = 1.0f / 3.0f;
        s->getMaterial().shininess         = 128.0f / 3.0f;
        s->getMaterial().transparency      = 7.0f / 9.0f;
        s->getMaterial().indexOfRefraction = 1.3100001f;
        a.addShape(s);

        Cube* c = new Cube(0.0f, 0.0f, 0.0f, 1.0f, 2, 2);
        c->useUniformScaling(false);
        c->setScale(1.0f / 3.0f, 16.0f / 7.0f, 0.6979999f);
        a.addShape(c);

        Light* L = new Light(1.0f / 3.0f, 2.0f / 3.0f, -1.0f / 9.0f, 3);
        L->setIntensity(1.0f / 7.0f);
        L->setLinearAttenuation(1.0f / 11.0f);
        a.addShape(L);
        a.setShapeCounter(4);

        Camera cam;
        SceneIO::save("/tmp/dirty_awkward.scene", a, cam);

        ShapeManager b;
        Camera cam2;
        SceneIO::load("/tmp/dirty_awkward.scene", b, cam2);

        unsigned long long ha = signature(a), hb = signature(b);
        check(ha == hb, "awkward float values round-trip to the same hash");
        if (ha != hb) {
            // Narrow it down so the failure names the guilty field.
            std::vector<Shape*>& A = a.getShapes();
            std::vector<Shape*>& B = b.getShapes();
            for (size_t i = 0; i < A.size() && i < B.size(); ++i) {
                if (A[i]->getX() != B[i]->getX()) std::printf("    shape %d: x %.9g -> %.9g\n", (int)i, A[i]->getX(), B[i]->getX());
                if (A[i]->getY() != B[i]->getY()) std::printf("    shape %d: y %.9g -> %.9g\n", (int)i, A[i]->getY(), B[i]->getY());
                if (A[i]->getScale() != B[i]->getScale()) std::printf("    shape %d: scale %.9g -> %.9g\n", (int)i, A[i]->getScale(), B[i]->getScale());
                glm::vec3 na = A[i]->getNonUniformScale(), nb = B[i]->getNonUniformScale();
                if (na != nb) std::printf("    shape %d: scaleXYZ %.9g %.9g %.9g -> %.9g %.9g %.9g\n", (int)i, na.x, na.y, na.z, nb.x, nb.y, nb.z);
                const Material& ma = A[i]->getMaterial();
                const Material& mb = B[i]->getMaterial();
                if (ma.ambient != mb.ambient) std::printf("    shape %d: ka %.9g -> %.9g\n", (int)i, ma.ambient, mb.ambient);
                if (ma.shininess != mb.shininess) std::printf("    shape %d: shininess %.9g -> %.9g\n", (int)i, ma.shininess, mb.shininess);
                if (ma.transparency != mb.transparency) std::printf("    shape %d: transparency %.9g -> %.9g\n", (int)i, ma.transparency, mb.transparency);
                const float* ca = A[i]->getCustomColor();
                const float* cb = B[i]->getCustomColor();
                for (int k = 0; k < 3; ++k)
                    if (ca[k] != cb[k]) std::printf("    shape %d: customColor[%d] %.9g -> %.9g\n", (int)i, k, ca[k], cb[k]);
            }
        }
    }

    // --- saving twice must produce byte-identical files ---------------------
    {
        ShapeManager a;
        a.addShape(new Cube(1.0f / 3.0f, 0.0f, 0.0f, 1.0f, 2, 1));
        // The counter must be consistent with the ids in use. load() raises it
        // to the highest id it finds - correct behaviour, but it means a scene
        // authored with a counter BELOW its own ids cannot be byte-stable, and
        // the editor never produces one.
        a.setShapeCounter(1);
        Camera cam;
        SceneIO::save("/tmp/dirty_twice_a.scene", a, cam);

        ShapeManager b;
        Camera cam2;
        SceneIO::load("/tmp/dirty_twice_a.scene", b, cam2);
        SceneIO::save("/tmp/dirty_twice_b.scene", b, cam2);

        FILE* f1 = std::fopen("/tmp/dirty_twice_a.scene", "rb");
        FILE* f2 = std::fopen("/tmp/dirty_twice_b.scene", "rb");
        bool same = f1 && f2;
        while (same) {
            int c1 = std::fgetc(f1), c2 = std::fgetc(f2);
            if (c1 != c2) { same = false; break; }
            if (c1 == EOF) break;
        }
        if (f1) std::fclose(f1);
        if (f2) std::fclose(f2);
        check(same, "save -> load -> save is byte-identical");
    }

    std::printf(failures ? "\n%d FAILED\n" : "\nall checks passed\n", failures);
    return failures ? 1 : 0;
}
