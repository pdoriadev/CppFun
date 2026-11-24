// Texture mapping -- assertions for CPSC 484 Assignment 5.
//
// Covers the three things that can independently be wrong:
//   1. the cache        decode, dedup, and bilinear sampling
//   2. the UVs          Sphere, Cube and OBJ vt parsing produce sane maps
//   3. the round trip    texture= survives save/load
//
// And one thing that must NOT change: an untextured scene must trace to
// exactly what it traced before texturing existed. raytrace_test's image sums
// cover that too; the check here is the local, obvious version of it.

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>

#include "gl_context.h"

#include "TextureCache.h"
#include "TexCoords.h"
#include "ImageIO.h"
#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "SceneIO.h"
#include "RayTracer.h"
#include "Cube.h"
#include "Sphere.h"
#include "Light.h"
#include "Pyramid.h"
#include "Icosahedron.h"
#include "Mobius.h"
#include "FileImporter.h"

static int failures = 0;
static int checks   = 0;

static void ok(bool cond, const std::string& what) {
    ++checks;
    if (!cond) { ++failures; std::printf("  FAIL  %s\n", what.c_str()); }
}

static void nearly(float a, float b, const std::string& what, float eps = 1e-3f) {
    ++checks;
    if (std::fabs(a - b) > eps) {
        ++failures;
        std::printf("  FAIL  %s   (got %.6f, expected %.6f)\n", what.c_str(), a, b);
    }
}

// A 4x4 image: left half pure red, right half pure blue. Written as BMP
// because ImageIO can already write one and stb_image can read it, so the test
// needs no binary fixture checked into the repo.
static const char* kImage = "texture_test_input.bmp";

static bool writeTestImage() {
    const int w = 4, h = 4;
    std::vector<unsigned char> px(static_cast<size_t>(w) * h * 3, 0);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const size_t o = (static_cast<size_t>(y) * w + x) * 3;
            if (x < w / 2) { px[o + 0] = 255; px[o + 1] = 0;   px[o + 2] = 0;   }
            else           { px[o + 0] = 0;   px[o + 1] = 0;   px[o + 2] = 255; }
        }
    }
    return ImageIO::writeBMP(kImage, px, w, h);
}

int main() {
    std::printf("Texture mapping\n");

    if (!writeTestImage()) {
        std::printf("could not write the test image\n");
        return 77;
    }

    // --- decode -------------------------------------------------------------
    {
        const Texture* t = TextureCache::get(kImage);
        ok(t != 0, "a BMP decodes");
        if (t) {
            ok(t->valid(), "the decoded texture reports valid");
            ok(t->width == 4 && t->height == 4, "dimensions come back right");
            ok(t->pixels.size() == 4u * 4u * 4u, "decoded to RGBA8");
        }
    }

    // --- dedup --------------------------------------------------------------
    {
        const size_t before = TextureCache::size();
        const Texture* a = TextureCache::get(kImage);
        const Texture* b = TextureCache::get(kImage);
        ok(a == b, "the same path returns the same object, not a second decode");
        ok(TextureCache::size() == before,
           "asking again does not grow the cache");

        TextureCache::get("definitely_not_a_file.png");
        ok(TextureCache::get("definitely_not_a_file.png") == 0,
           "a missing file returns null");
        ok(!TextureCache::lastError().empty(), "and records why");
    }

    // --- sampling -----------------------------------------------------------
    {
        const Texture* t = TextureCache::get(kImage);
        ok(t && t->valid(), "texture available for sampling");
        if (t && t->valid()) {
            // Texel centres of a 4-wide image are at u = 1/8, 3/8, 5/8, 7/8.
            // Sampling there avoids the bilinear blend and gives pure colours.
            glm::vec3 left  = t->sample(0.125f, 0.5f);
            glm::vec3 right = t->sample(0.875f, 0.5f);
            nearly(left.r,  1.0f, "left half samples red (r)");
            nearly(left.b,  0.0f, "left half samples red (b)");
            nearly(right.r, 0.0f, "right half samples blue (r)");
            nearly(right.b, 1.0f, "right half samples blue (b)");

            // BILINEAR. Everything above samples texel CENTRES, where nearest
            // and bilinear agree exactly -- so none of it can tell the two
            // apart, and a nearest-neighbour sampler passes every one.
            //
            // The red/blue boundary lies between texel 1 (centre u = 3/8) and
            // texel 2 (centre u = 5/8). Sampling between those centres is the
            // only place the blend is observable.
            glm::vec3 mid = t->sample(0.5f, 0.5f);       // exactly halfway
            nearly(mid.r, 0.5f,
                   "halfway between two texels blends them -- BILINEAR, not "
                   "nearest (a nearest sampler returns pure red or pure blue)",
                   0.02f);
            nearly(mid.b, 0.5f, "and the blend is symmetric", 0.02f);

            // Quarter of the way, so "just average the two neighbours" fails
            // as well: the weights have to follow the distance.
            glm::vec3 quarter = t->sample(0.4375f, 0.5f);
            nearly(quarter.r, 0.75f,
                   "a quarter of the way across weights the NEARER texel more "
                   "-- the blend is distance-weighted, not a flat average",
                   0.02f);
            nearly(quarter.b, 0.25f, "and the far texel correspondingly less", 0.02f);

            // WRAPPING, asserted against absolute colours rather than against
            // another sample. Comparing sample(1.125) to sample(0.125) passes
            // for any function that ignores u entirely -- including a stub that
            // returns a constant -- so it proved nothing.
            glm::vec3 wrapped = t->sample(1.625f, 0.5f);
            nearly(wrapped.b, 1.0f,
                   "u = 1.625 wraps to 0.625 and samples BLUE -- wrapping, not "
                   "clamping to the right edge");
            nearly(wrapped.r, 0.0f, "and no red bleeds in");

            glm::vec3 negative = t->sample(-0.375f, 0.5f);
            nearly(negative.b, 1.0f,
                   "u = -0.375 wraps forward to 0.625 and samples blue too -- "
                   "negative u must wrap, not clamp to 0");
            nearly(negative.r, 0.0f, "and no red there either");
        }
    }

    // --- cylindrical projection, and its seam --------------------------------
    //
    // Ungraded until now, which is half the file's student code and the one
    // piece with a real trap in it. No GL needed: pure geometry.
    {
        // A closed ring about the Y axis, two rows tall. Some of its triangles
        // necessarily straddle the -X axis, where atan2 flips from +pi to -pi.
        const int N = 12;
        std::vector<glm::vec3> verts;
        for (int row = 0; row < 2; ++row) {
            for (int i = 0; i < N; ++i) {
                const float a = 6.28318530718f * static_cast<float>(i) / N;
                verts.push_back(glm::vec3(std::cos(a), static_cast<float>(row),
                                          std::sin(a)));
            }
        }
        std::vector<std::vector<int> > faces;
        for (int i = 0; i < N; ++i) {
            const int a = i, b = (i + 1) % N, c = a + N, d = b + N;
            std::vector<int> t1; t1.push_back(a); t1.push_back(b); t1.push_back(c);
            std::vector<int> t2; t2.push_back(b); t2.push_back(d); t2.push_back(c);
            faces.push_back(t1); faces.push_back(t2);
        }

        const std::vector<glm::vec2> uv = TexCoords::cylindricalPerCorner(verts, faces);

        ok(uv.size() == faces.size() * 3,
           "cylindrical: one UV per triangle CORNER (faces * 3) -- per-corner "
           "is the whole point; a per-vertex map cannot express the seam");

        if (uv.size() == faces.size() * 3) {
            float vLo = uv[0].y, vHi = uv[0].y;
            for (size_t i = 1; i < uv.size(); ++i) {
                vLo = std::min(vLo, uv[i].y);
                vHi = std::max(vHi, uv[i].y);
            }
            ok(vLo < 0.05f && vHi > 0.95f,
               "cylindrical: v spans the mesh's own height, bottom row to top");

            // THE SEAM. A triangle crossing the back of the mesh has raw
            // corners at u = 0.98 and u = 0.02. Left alone, the interpolation
            // runs the long way round and squeezes the entire image, backwards,
            // into that one triangle -- a bright vertical scar down the back.
            // Repaired, no triangle spans more than a fraction of the range.
            float worst = 0.0f;
            int   worstFace = -1;
            for (size_t f = 0; f < faces.size(); ++f) {
                const glm::vec2& a = uv[f * 3 + 0];
                const glm::vec2& b = uv[f * 3 + 1];
                const glm::vec2& c = uv[f * 3 + 2];
                const float hi = std::max(a.x, std::max(b.x, c.x));
                const float lo = std::min(a.x, std::min(b.x, c.x));
                if (hi - lo > worst) { worst = hi - lo; worstFace = static_cast<int>(f); }
            }
            ok(worst < 0.5f,
               "cylindrical: NO triangle spans more than half the u range -- "
               "the wrap seam is repaired by pushing the low corners past 1.0, "
               "not left to smear the whole texture across one face");
            if (worst >= 0.5f) {
                std::printf("         worst face %d spans %.3f in u\n", worstFace, worst);
            }

            // And the projection has to actually go round: a map that collapses
            // every corner onto one u would pass the seam check trivially.
            float uLo = uv[0].x, uHi = uv[0].x;
            for (size_t i = 1; i < uv.size(); ++i) {
                uLo = std::min(uLo, uv[i].x);
                uHi = std::max(uHi, uv[i].x);
            }
            ok(uHi - uLo > 0.8f,
               "cylindrical: u actually travels round the axis rather than "
               "collapsing -- a constant u would satisfy the seam check for "
               "the wrong reason");
        } else {
            checks += 3; failures += 3;
        }

        ok(TexCoords::cylindricalPerCorner(std::vector<glm::vec3>(),
                                           std::vector<std::vector<int> >()).empty(),
           "cylindrical: an empty mesh gives an empty result rather than "
           "indexing off the end");
    }

    if (!makeHeadlessContext()) {
        std::printf("no GL context -- geometry and render checks skipped\n");
        std::printf("\n%d checks, %d failures\n", checks, failures);
        return failures == 0 ? 0 : 1;
    }

    // --- UV generation ------------------------------------------------------
    {
        Sphere s(0, 0, 0, 1.0f, 2, 1);
        ok(s.hasTexCoords(), "Sphere generates texture coordinates");

        bool inRange = true;
        for (size_t i = 0; i < s.getTexCoords().size(); ++i) {
            const glm::vec2& uv = s.getTexCoords()[i];
            if (uv.x < -1e-6f || uv.x > 1.0f + 1e-6f ||
                uv.y < -1e-6f || uv.y > 1.0f + 1e-6f) { inRange = false; break; }
        }
        ok(inRange, "every Sphere UV lies inside [0,1]");

        // --- handedness ---------------------------------------------------
        //
        // Regression. Sphere::setupSphere sweeps longitude as
        // x = cos(2*pi*s), z = sin(2*pi*s), rotating +X toward +Z, so
        // increasing s moves LEFT across the surface seen from outside.
        // Mapping u straight onto s mirrored every texture -- invisible on a
        // checkerboard, and obvious the moment the image contains text or a
        // map, which then read backwards.
        //
        // Walked over TRIANGLE CORNERS, because texCoords is per corner:
        // corner k of face f is vertices[faces[f][k]] with texCoords[f*3+k].
        // Indexing UVs by vertex is exactly the mistake this suite exists to
        // catch.
        //
        // Checked geometrically rather than by asserting a formula, so it
        // still holds if the parameterisation is rewritten.
        const std::vector<glm::vec3>&          sv  = s.getVertices();
        const std::vector<glm::vec2>&          suv = s.getTexCoords();
        const std::vector<std::vector<int> >&  sf  = s.getFaces();

        int  found = 0;
        bool handedness = true;
        for (size_t f = 0; f < sf.size() && found < 12; ++f) {
            for (int k = 0; k < 2; ++k) {
                const glm::vec3& p0 = sv[static_cast<size_t>(sf[f][k])];
                const glm::vec3& p1 = sv[static_cast<size_t>(sf[f][k + 1])];
                const glm::vec2& t0 = suv[f * 3 + static_cast<size_t>(k)];
                const glm::vec2& t1 = suv[f * 3 + static_cast<size_t>(k) + 1];

                // Near the equator, on the same latitude ring, away from the
                // wrap seam.
                if (std::fabs(p0.y) > 0.05f) continue;
                if (std::fabs(p1.y - p0.y) > 1e-4f) continue;
                const float du = t1.x - t0.x;
                if (std::fabs(du) < 1e-6f || std::fabs(du) > 0.5f) continue;

                // Cross product about Y says which way round the ring we went.
                const float turn = p0.x * p1.z - p0.z * p1.x;
                if (turn > 0.0f && du > 0.0f) handedness = false;
                if (turn < 0.0f && du < 0.0f) handedness = false;
                ++found;
            }
        }
        ok(found > 0, "found equator corners to check handedness against");
        ok(handedness,
           "u runs the right way round the sphere -- a mirrored map would read "
           "backwards on any texture containing text");

        // The poles: v = 1 must be the top of the image (+y), v = 0 the bottom.
        float vAtTop = -1.0f, vAtBottom = -1.0f;
        for (size_t f = 0; f < sf.size(); ++f) {
            for (int k = 0; k < 3; ++k) {
                const glm::vec3& p = sv[static_cast<size_t>(sf[f][k])];
                if (p.y >  0.499f) vAtTop    = suv[f * 3 + static_cast<size_t>(k)].y;
                if (p.y < -0.499f) vAtBottom = suv[f * 3 + static_cast<size_t>(k)].y;
            }
        }
        nearly(vAtTop, 1.0f, "the north pole samples the top of the image");
        nearly(vAtBottom, 0.0f, "the south pole samples the bottom");
    }
    // --- the invariant that matters most -----------------------------------
    //
    // Every shape expands its faces into three fresh vertices per triangle and
    // uses a sequential index buffer, so the UV array uploaded to attribute 3
    // must be exactly faces.size() * 3 long.
    //
    // Regression. Sphere returned one UV per UNIQUE vertex -- 441 against 2400
    // emitted -- so attribute 3 ran off the end of a too-short buffer after the
    // first ring and every vertex past it read zero. Completely invisible in a
    // ray-traced render, because the tracer reads the CPU arrays, which were
    // correct; only the rasterized viewport was wrong.
    {
        struct Case { const char* name; Shape* shape; };
        Sphere      sp(0, 0, 0, 1.0f, 2, 90);
        Cube        cb(0, 0, 0, 1.0f, 2, 91);
        Pyramid     py(0, 0, 0, 1.0f, 2, 92);
        Icosahedron ic(0, 0, 0, 1.0f, 2, 93);
        Mobius      mo(0, 0, 0, 1.0f, 2, 94);

        Case cases[] = { {"Sphere", &sp}, {"Cube", &cb}, {"Pyramid", &py},
                         {"Icosahedron", &ic}, {"Mobius", &mo} };

        for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
            Shape* sh = cases[i].shape;
            const std::string n = cases[i].name;

            ok(sh->hasTexCoords(), n + " generates texture coordinates");
            ok(sh->getTexCoords().size() == sh->getFaces().size() * 3,
               n + ": one UV per emitted triangle corner (faces * 3), which is "
                   "what the GL buffer actually contains");

            bool inRange = true;
            for (size_t k = 0; k < sh->getTexCoords().size(); ++k) {
                const glm::vec2& uv = sh->getTexCoords()[k];
                if (uv.x < -1e-4f || uv.x > 1.0f + 1e-4f ||
                    uv.y < -1e-4f || uv.y > 1.0f + 1e-4f) { inRange = false; break; }
            }
            ok(inRange, n + ": every UV lies inside [0,1]");

            // A map that never varies is a map that does nothing.
            float mnU = 2.0f, mxU = -1.0f, mnV = 2.0f, mxV = -1.0f;
            for (size_t k = 0; k < sh->getTexCoords().size(); ++k) {
                mnU = std::min(mnU, sh->getTexCoords()[k].x);
                mxU = std::max(mxU, sh->getTexCoords()[k].x);
                mnV = std::min(mnV, sh->getTexCoords()[k].y);
                mxV = std::max(mxV, sh->getTexCoords()[k].y);
            }
            ok(mxU - mnU > 0.5f && mxV - mnV > 0.5f,
               n + ": the UVs actually span the image rather than collapsing");

            // Generating UVs is half the job; uploading them is the other
            // half, and forgetting the second half is invisible to every
            // other assertion here and to the whole ray tracer.
            ok(sh->getTexVBO() != 0,
               n + ": uploadTexCoords() ran, so attribute 3 is actually fed");
        }
    }

    {
        Cube c(0, 0, 0, 1.0f, 2, 2);
        ok(c.hasTexCoords(), "Cube generates texture coordinates");
        // The classic trap: 8 shared corners cannot carry per-face UVs. The
        // cube sidesteps it by emitting one vertex per triangle corner, so the
        // UV count follows the FACES, not the vertices.
        ok(c.getTexCoords().size() == c.getFaces().size() * 3,
           "Cube's UVs are per triangle corner (12 faces x 3 = 36)");
        ok(c.getTexCoords().size() != c.getVertices().size(),
           "and are deliberately NOT per vertex -- a corner belongs to 3 faces");

        // Each face must cover the full 0..1 square, or the texture appears
        // cropped on that face.
        float mn = 2.0f, mx = -1.0f;
        for (size_t i = 0; i < c.getTexCoords().size(); ++i) {
            mn = std::min(mn, std::min(c.getTexCoords()[i].x, c.getTexCoords()[i].y));
            mx = std::max(mx, std::max(c.getTexCoords()[i].x, c.getTexCoords()[i].y));
        }
        nearly(mn, 0.0f, "Cube UVs reach 0");
        nearly(mx, 1.0f, "Cube UVs reach 1");
    }

    // --- OBJ vt parsing -----------------------------------------------------
    {
        const char* objPath = "texture_test_quad.obj";
        {
            std::ofstream f(objPath);
            f << "v -1 -1 0\nv 1 -1 0\nv 1 1 0\nv -1 1 0\n";
            f << "vt 0 0\nvt 1 0\nvt 1 1\nvt 0 1\n";
            f << "vn 0 0 1\n";
            f << "f 1/1/1 2/2/1 3/3/1\n";
            f << "f 1/1/1 3/3/1 4/4/1\n";
        }

        ShapeManager sm;
        FileImporter imp;
        imp.setPendingPath(objPath);
        const int rc = imp.importObjFile(sm);

        // Skipped rather than failed when the OBJ parser is stubbed out.
        // This suite grades TEXTURES; a student whose objparser assignment is
        // unfinished -- or a course where objparser is the assignment and
        // texturing is given -- must not lose marks here for it. See the
        // "Grading an assignment in isolation" section of ASSIGNMENTS.md.
        if (rc != 1 || !sm.getSelectedShape()) {
            std::printf("  (OBJ parser unavailable -- vt checks skipped)\n");
        } else {
            Shape* s = sm.getSelectedShape();
            ok(s->hasTexCoords(), "vt lines become texture coordinates");
            ok(s->getTexCoords().size() == s->getFaces().size() * 3,
               "OBJ UVs are per triangle corner, matching the emitted vertices");
            if (s->getTexCoords().size() >= 3) {
                nearly(s->getTexCoords()[0].x, 0.0f, "first corner u");
                nearly(s->getTexCoords()[1].x, 1.0f, "second corner u");
                nearly(s->getTexCoords()[2].y, 1.0f, "third corner v");
            }
        }
        std::remove(objPath);

        // And a mesh with NO vt must stay untextured, so attribute 3 is left
        // disabled and untextured rendering is unaffected.
        const char* plain = "texture_test_plain.obj";
        {
            std::ofstream f(plain);
            f << "v -1 -1 0\nv 1 -1 0\nv 1 1 0\nvn 0 0 1\n";
            f << "f 1//1 2//1 3//1\n";
        }
        ShapeManager sm2;
        FileImporter imp2;
        imp2.setPendingPath(plain);
        imp2.importObjFile(sm2);
        if (sm2.getSelectedShape()) {
            ok(!sm2.getSelectedShape()->hasTexCoords(),
               "an OBJ without vt lines stays untextured");
        }
        std::remove(plain);
    }

    // --- scene round trip ---------------------------------------------------
    {
        ShapeManager sm;
        Camera cam;
        Cube* c = new Cube(0, 0, 0, 1.0f, 2, 5);
        c->getMaterial().texturePath = kImage;
        sm.addShape(c);
        sm.setShapeCounter(6);

        const std::string path = "texture_roundtrip.scene";
        ok(SceneIO::save(path, sm, cam), "a scene with a texture saves");

        ShapeManager sm2;
        Camera cam2;
        SceneIO::Result r = SceneIO::load(path, sm2, cam2);
        ok(r.ok, "it loads back");
        ok(!sm2.getShapes().empty(), "the shape came back");
        if (!sm2.getShapes().empty()) {
            const std::string got = sm2.getShapes()[0]->getMaterial().texturePath;
            ok(!got.empty(), "the texture path came back");
            ok(got.find("texture_test_input.bmp") != std::string::npos,
               "and points at the same file");
        }
        std::remove(path.c_str());
    }

    // --- the tracer actually samples it -------------------------------------
    //
    // Two traces of the same scene, one textured and one not. The textured one
    // must differ; the untextured one must be EXACTLY what it was before
    // texturing existed, which is the guarantee the whole design hangs on.
    {
        ShapeManager sm;
        Cube* c = new Cube(0.0f, 0.0f, 0.0f, 1.6f, 31, 1);
        c->setCustomColor(1.0f, 1.0f, 1.0f);
        sm.addShape(c);
        sm.addShape(new Light(3.0f, 4.0f, 5.0f, 2));
        sm.setShapeCounter(3);

        Camera cam;
        cam.setView(3.14159265f / 2.0f + 0.4f, 0.3f, 4.0f);

        RayTraceSettings s;
        s.mode            = RayTraceSettings::Mode_RayCast;
        s.width           = 64;
        s.height          = 64;
        s.samplesPerPixel = 1;
        s.background      = glm::vec3(0.0f);

        struct Trace {
            static long sum(ShapeManager& sm, const Camera& cam,
                            const RayTraceSettings& s) {
                RayTracer t;
                t.begin(cam, sm, s);
                int guard = 0;
                while (t.step(1e9) && guard++ < 1000) {}
                long acc = 0;
                const std::vector<unsigned char>& px = t.getPixels();
                for (size_t i = 0; i < px.size(); ++i) acc += px[i];
                return acc;
            }
        };

        const long plain = Trace::sum(sm, cam, s);

        // Same reasoning as the OBJ block above: these assertions need a
        // working shade(), which is the RAYTRACER assignment, not this one.
        // A stubbed shade() returns black, so the whole image sums to zero.
        if (plain <= 0) {
            std::printf("  (ray tracer shading unavailable -- render checks skipped)\n");
        } else {
            c->getMaterial().texturePath = kImage;
            const long textured = Trace::sum(sm, cam, s);
            c->getMaterial().texturePath.clear();
            const long plainAgain = Trace::sum(sm, cam, s);

            ok(textured != plain, "assigning a texture changes the traced image");
            ok(plainAgain == plain,
               "clearing the texture restores the exact original image");

            // The 4x4 image is half pure red and half pure blue, so a white
            // cube under it must lose green everywhere the texture is sampled.
            ok(textured < plain,
               "a red/blue texture on a white cube darkens it (green is zero in the map)");
        }
    }

    std::remove(kImage);

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) { std::printf("ALL TESTS PASSED (0 failures)\n"); return 0; }
    std::printf("TESTS FAILED (%d failures)\n", failures);
    return 1;
}
