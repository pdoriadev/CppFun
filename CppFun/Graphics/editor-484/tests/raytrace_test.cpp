// Headless correctness checks for the ray casting / ray tracing core.
// Builds a tiny scene from plain Shape subclasses (no GL calls), renders it,
// and asserts on the resulting pixels.
//
//   make -f tests/Makefile.test && ./raytrace_test

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

#include "Shape.h"
#include "ShapeManager.h"
#include "Camera.h"
#include "RayScene.h"
#include "RayTracer.h"
#include "SceneLight.h"
#include "Light.h"
#include "Material.h"
#include "ImageIO.h"

static int failures = 0;
static int checksRun = 0;

static void check(bool condition, const char* what) {
    ++checksRun;
    if (condition) {
        std::printf("  PASS  %s\n", what);
    } else {
        std::printf("  FAIL  %s\n", what);
        ++failures;
    }
}

// A Shape that carries geometry but never touches OpenGL.
class TestShape : public Shape {
public:
    TestShape(float x, float y, float z, float scale, int colorIndex, int id)
        : Shape(x, y, z, scale, colorIndex, id) {
        setShapeType("TestShape");
    }
    void draw(GLuint) override {}   // never called in these tests
};


// Mimics ImportShape, which stores faces as interleaved vertex/normal index
// pairs rather than plain vertex indices.
class InterleavedShape : public Shape {
public:
    InterleavedShape(float x, float y, float z, float scale, int colorIndex, int id)
        : Shape(x, y, z, scale, colorIndex, id) {}
    void draw(GLuint) override {}
    int getFaceIndexStride() const override { return 2; }
};

// Axis-aligned box centered on the origin, side length 1.
static TestShape* makeBox(float x, float y, float z, float scale, int colorIndex, int id) {
    TestShape* s = new TestShape(x, y, z, scale, colorIndex, id);

    const float h = 0.5f;
    std::vector<glm::vec3> v;
    v.push_back(glm::vec3(-h, -h, -h)); v.push_back(glm::vec3( h, -h, -h));
    v.push_back(glm::vec3( h,  h, -h)); v.push_back(glm::vec3(-h,  h, -h));
    v.push_back(glm::vec3(-h, -h,  h)); v.push_back(glm::vec3( h, -h,  h));
    v.push_back(glm::vec3( h,  h,  h)); v.push_back(glm::vec3(-h,  h,  h));
    s->setVertices(v);

    const int quads[6][4] = {
        {4,5,6,7}, {1,0,3,2}, {0,4,7,3}, {5,1,2,6}, {3,7,6,2}, {0,1,5,4}
    };
    std::vector<std::vector<int> > faces;
    for (int q = 0; q < 6; ++q) {
        std::vector<int> f;
        for (int k = 0; k < 4; ++k) f.push_back(quads[q][k]);
        faces.push_back(f);   // quads exercise the fan-triangulation path
    }
    s->setFaces(faces);
    return s;
}

// Large flat quad in the y = value plane, to catch shadows.
static TestShape* makeFloor(float y, float halfSize, int colorIndex, int id) {
    TestShape* s = new TestShape(0.0f, y, 0.0f, 1.0f, colorIndex, id);

    std::vector<glm::vec3> v;
    v.push_back(glm::vec3(-halfSize, 0.0f, -halfSize));
    v.push_back(glm::vec3( halfSize, 0.0f, -halfSize));
    v.push_back(glm::vec3( halfSize, 0.0f,  halfSize));
    v.push_back(glm::vec3(-halfSize, 0.0f,  halfSize));
    s->setVertices(v);

    std::vector<std::vector<int> > faces;
    std::vector<int> f;
    f.push_back(0); f.push_back(1); f.push_back(2); f.push_back(3);
    faces.push_back(f);
    s->setFaces(faces);
    return s;
}

// Render a scene to completion and return the sum of every channel byte.
// A crude image fingerprint, but enough to tell "this setting changed the
// picture" from "this setting was ignored".
static long renderSum(Camera& cam, ShapeManager& sm, const RayTraceSettings& rs) {
    RayTracer t;
    t.begin(cam, sm, rs);
    int guard = 0;
    while (t.step(2.0) && guard++ < 4000) {}
    long sum = 0;
    const std::vector<unsigned char>& px = t.getPixels();
    for (size_t i = 0; i < px.size(); ++i) sum += px[i];
    return sum;
}

int main() {
    std::printf("Ray tracing core tests\n\n");

    // ---------------------------------------------------------- geometry ----
    std::printf("Scene construction\n");
    ShapeManager sm;
    TestShape* box   = makeBox(0.0f, 0.0f, 0.0f, 1.0f, 2, 1);
    TestShape* floor = makeFloor(-1.0f, 2.6f, 7, 2);
    sm.addShape(box);
    sm.addShape(floor);

    RayScene scene;
    scene.build(sm);

    // 6 quads -> 12 triangles, plus 1 quad floor -> 2 triangles
    check(scene.triangleCount() == 14, "quad faces fan-triangulate (14 triangles)");
    check(scene.shapeCount() == 2, "both shapes contribute geometry");

    // ------------------------------------------------------ dependency ----
    // This suite grades the SHADING half of Assignment 6. Nearly every check
    // in it needs a ray to be able to find a triangle first, and that is the
    // BVH half -- a different file, a different grader. With the acceleration
    // structure stubbed, every one of those checks fails for a reason that has
    // nothing to do with the code they are pointed at, which sends students
    // hunting through the wrong file.
    //
    // So probe once. If a ray cannot hit a box that is squarely in front of
    // it, skip the dependent checks and say plainly which half to fix.
    bool haveIntersector = false;
    {
        Ray probeRay(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        Hit probeHit;
        haveIntersector = scene.intersect(probeRay, 1e-4f, 1e30f, probeHit)
                          && probeHit.shape == box;
    }
    if (!haveIntersector) {
        std::printf(
            "\n>>> RayScene::intersect cannot find a triangle that is directly\n"
            ">>> in front of the ray, so every check below that needs a hit is\n"
            ">>> SKIPPED -- they would all fail for the same single reason.\n"
            ">>>\n"
            ">>> That is the BVH half of this assignment, not the shading half.\n"
            ">>> Fix src/BVH.cpp and run ./bvh_test until it is green; this\n"
            ">>> suite cannot grade anything until a ray can find geometry.\n");
    }

    // ------------------------------------------------------- intersection ----
    std::printf("\nRay/scene intersection\n");
    if (haveIntersector) {
        // Straight down the -Z axis at the box, which spans [-0.5, 0.5].
        Ray ray(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        Hit hit;
        bool got = scene.intersect(ray, 1e-4f, 1e30f, hit);
        check(got, "ray hits the box");
        check(got && hit.shape == box, "hit reports the correct shape");
        check(got && std::fabs(hit.t - 4.5f) < 1e-3f, "hit distance is the near face (t = 4.5)");
        check(got && hit.normal.z > 0.9f, "normal faces back toward the ray");
        check(got && !hit.backface, "front face is not flagged as a backface");
    }
    if (haveIntersector) {
        // Aimed away from everything.
        Ray ray(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        Hit hit;
        check(!scene.intersect(ray, 1e-4f, 1e30f, hit), "ray aimed at nothing misses");
    }
    if (haveIntersector) {
        // From inside the box: should exit through a backface.
        Ray ray(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        Hit hit;
        bool got = scene.intersect(ray, 1e-4f, 1e30f, hit);
        check(got && hit.backface, "ray from inside exits through a backface");
    }

    // ------------------------------------------------------------ shadows ----
    std::printf("\nOcclusion\n");
    if (haveIntersector) {
        // The light sits at +Y/+Z, so the box's shadow lands on the floor
        // around z = -2, NOT directly underneath it.
        glm::vec3 inShadow(0.0f, -0.999f, -2.0f);
        glm::vec3 toLight = glm::normalize(g_sceneLight.position - inShadow);
        Ray shadowRay(inShadow + glm::vec3(0.0f, 1e-3f, 0.0f), toLight);
        float dist = glm::length(g_sceneLight.position - inShadow);
        check(scene.occluded(shadowRay, 1e-4f, dist - 1e-3f, 0),
              "floor point inside the cast shadow is occluded");

        // Far corner of the floor has a clear line to the light.
        glm::vec3 farCorner(5.0f, -0.999f, 4.0f);
        glm::vec3 toLight2 = glm::normalize(g_sceneLight.position - farCorner);
        Ray clearRay(farCorner + glm::vec3(0.0f, 1e-3f, 0.0f), toLight2);
        float dist2 = glm::length(g_sceneLight.position - farCorner);
        check(!scene.occluded(clearRay, 1e-4f, dist2 - 1e-3f, 0),
              "open floor point is not occluded");
    }

    // ------------------------------------------------------ camera / rays ----
    std::printf("\nCamera ray generation\n");
    {
        Camera cam;
        cam.setView(3.14159265f / 2.0f, 0.0f, 5.0f);   // looking down -Z from +Z

        const int W = 200, H = 100;
        Ray center = cam.generateRay(W * 0.5f, H * 0.5f, W, H);
        glm::vec3 toTarget = glm::normalize(cam.getTarget() - cam.getPosition());
        check(glm::length(center.direction - toTarget) < 1e-3f,
              "center pixel ray points at the camera target");

        Ray left  = cam.generateRay(0.0f, H * 0.5f, W, H);
        Ray right = cam.generateRay(static_cast<float>(W), H * 0.5f, W, H);
        check(left.direction.x != right.direction.x, "left and right rays diverge");

        Ray top    = cam.generateRay(W * 0.5f, 0.0f, W, H);
        Ray bottom = cam.generateRay(W * 0.5f, static_cast<float>(H), W, H);
        check(top.direction.y > bottom.direction.y,
              "top-of-image ray points higher than bottom (Y not flipped)");

        check(std::fabs(glm::length(center.direction) - 1.0f) < 1e-5f,
              "generated directions are normalized");
    }

    // ------------------------------------------------------------- render ----
    std::printf("\nRendering\n");
    if (haveIntersector) {
        Camera cam;
        // Elevated view so the floor behind the box (where the shadow
        // falls) is actually visible to the camera.
        cam.setView(3.14159265f / 2.0f, 1.0f, 6.0f);

        RayTraceSettings settings;
        settings.width = 160;
        settings.height = 120;
        settings.shadows = true;
        settings.reflections = true;
        settings.refractions = true;
        settings.maxDepth = 3;
        settings.background = glm::vec3(0.0f, 0.0f, 0.0f);

        box->setReflectivity(0.4f);

        RayTracer tracer;
        tracer.begin(cam, sm, settings);

        int guard = 0;
        while (tracer.step(1.0) && guard++ < 1000) {}

        check(!tracer.isRendering(), "render runs to completion");
        check(tracer.progress() >= 1.0f, "progress reaches 100%");
        check(tracer.hasImage(), "image buffer is populated");

        const std::vector<unsigned char>& px = tracer.getPixels();
        check(px.size() == static_cast<size_t>(160 * 120 * 3), "buffer is width*height*3");

        long litPixels = 0, blackPixels = 0;
        for (size_t i = 0; i < px.size(); i += 3) {
            int sum = px[i] + px[i + 1] + px[i + 2];
            if (sum == 0) ++blackPixels; else ++litPixels;
        }
        check(litPixels > 0, "some pixels show geometry");
        check(blackPixels > 0, "some pixels show background");

        std::printf("        %ld lit / %ld background\n", litPixels, blackPixels);

        // Shadow check: with the light at +Y/+Z, the floor area directly behind
        // the box should be darker than open floor off to the side.
        RayTraceSettings noShadow = settings;
        noShadow.shadows = false;
        RayTracer plain;
        plain.begin(cam, sm, noShadow);
        guard = 0;
        while (plain.step(1.0) && guard++ < 1000) {}

        long shadowedSum = 0, unshadowedSum = 0;
        const std::vector<unsigned char>& a = tracer.getPixels();
        const std::vector<unsigned char>& b = plain.getPixels();
        for (size_t i = 0; i < a.size(); ++i) {
            shadowedSum   += a[i];
            unshadowedSum += b[i];
        }
        std::printf("        shadowed sum %ld vs unshadowed %ld\n", shadowedSum, unshadowedSum);
        check(shadowedSum < unshadowedSum,
              "enabling shadows darkens the image overall");

        check(tracer.saveImage("test_render.ppm"), "writes a PPM");
        check(tracer.saveImage("test_render.bmp"), "writes a BMP");
    }

    // ------------------------------------------- recursion and blending ----
    // Two things go wrong in almost every first reflection implementation, and
    // neither of them looks obviously wrong on screen:
    //
    //   1. maxDepth is tested AFTER the first reflection ray is cast, so
    //      "depth 0" still renders one bounce and the budget is off by one.
    //   2. the reflected colour REPLACES the surface colour instead of being
    //      blended with it, so a 30%-reflective object renders as a mirror.
    //
    // Both are caught by comparing whole images rather than single pixels.
    std::printf("\nRecursion depth and reflectivity blending\n");
    if (haveIntersector) {
        ShapeManager rsm;
        TestShape* mirror = makeBox(0.0f, 0.0f, 0.0f, 1.0f, 2, 1);
        rsm.addShape(mirror);
        rsm.addShape(makeFloor(-1.0f, 4.0f, 7, 2));

        Camera cam;
        cam.setView(3.14159265f / 2.0f, 0.6f, 5.0f);

        RayTraceSettings base;
        base.width = 96;
        base.height = 72;
        base.shadows = false;        // isolate reflection
        base.refractions = false;
        base.reflections = true;
        base.background = glm::vec3(0.0f, 0.0f, 0.0f);

        RayTraceSettings off = base; off.reflections = false;
        RayTraceSettings d0  = base; d0.maxDepth = 0;
        RayTraceSettings d1  = base; d1.maxDepth = 1;
        RayTraceSettings d3  = base; d3.maxDepth = 3;

        mirror->setReflectivity(1.0f);
        const long sumOff = renderSum(cam, rsm, off);
        const long sumD0  = renderSum(cam, rsm, d0);
        const long sumD1  = renderSum(cam, rsm, d1);

        std::printf("        reflections off %ld | depth 0 %ld | depth 1 %ld\n",
                    sumOff, sumD0, sumD1);

        // NOTE: the depth-0 check below is gated on this one. Against a stub
        // that returns a constant colour every render is identical, so
        // "depth 0 matches reflections-off" would pass vacuously and hide a
        // completely missing implementation. It only means anything once
        // reflections have been shown to change the image at all.
        const bool reflectionsDoSomething = (sumD1 != sumD0);
        check(reflectionsDoSomething,
              "raising maxDepth from 0 to 1 changes the image -- the setting is "
              "actually consulted, not ignored");
        check(reflectionsDoSomething && sumD0 == sumOff,
              "maxDepth = 0 then renders exactly as reflections-off -- the depth "
              "test must be checked BEFORE the first reflection ray is cast");

        mirror->setReflectivity(0.0f);
        const long matte = renderSum(cam, rsm, d3);
        mirror->setReflectivity(0.5f);
        const long half  = renderSum(cam, rsm, d3);
        mirror->setReflectivity(1.0f);
        const long full  = renderSum(cam, rsm, d3);

        std::printf("        reflectivity 0.0 %ld | 0.5 %ld | 1.0 %ld\n",
                    matte, half, full);

        const bool reflectivityDoesSomething = (matte != full);
        check(reflectivityDoesSomething,
              "changing a material's reflectivity changes the image");
        check(reflectivityDoesSomething &&
              ((half > matte && half < full) || (half < matte && half > full)),
              "reflectivity 0.5 lands strictly between matte and mirror -- the "
              "reflected colour is BLENDED with the surface colour, not "
              "substituted for it");
    }

    // -------------------------------------------------------- refraction ----
    // Transmission is hard to eyeball, so verify it numerically: put a red
    // backdrop behind a transparent box with IOR 1.0. At IOR 1.0 the ray must
    // pass through undeviated, so the camera should see the backdrop's red,
    // not the box's green. Flipping transparency to 0 must show green instead.
    std::printf("\nRefraction\n");
    if (haveIntersector) {
        ShapeManager rsm;

        TestShape* backdrop = new TestShape(0.0f, 0.0f, -3.0f, 1.0f, 31, 1);
        {
            std::vector<glm::vec3> v;
            v.push_back(glm::vec3(-3.0f, -3.0f, 0.0f));
            v.push_back(glm::vec3( 3.0f, -3.0f, 0.0f));
            v.push_back(glm::vec3( 3.0f,  3.0f, 0.0f));
            v.push_back(glm::vec3(-3.0f,  3.0f, 0.0f));
            backdrop->setVertices(v);
            std::vector<std::vector<int> > f;
            std::vector<int> q;
            q.push_back(0); q.push_back(1); q.push_back(2); q.push_back(3);
            f.push_back(q);
            backdrop->setFaces(f);
            backdrop->setCustomColor(1.0f, 0.0f, 0.0f);
        }
        rsm.addShape(backdrop);

        TestShape* glassBox = makeBox(0.0f, 0.0f, 0.0f, 1.0f, 31, 2);
        glassBox->setCustomColor(0.0f, 1.0f, 0.0f);
        glassBox->setIndexOfRefraction(1.0f);
        rsm.addShape(glassBox);

        Camera cam;
        cam.setView(3.14159265f / 2.0f, 0.0f, 5.0f);

        RayTraceSettings rs;
        rs.width = 64;
        rs.height = 64;
        rs.maxDepth = 8;
        rs.shadows = false;          // isolate transmission
        rs.reflections = false;
        rs.refractions = true;
        rs.background = glm::vec3(0.0f, 0.0f, 1.0f);   // blue: must not show through

        const size_t centerIndex = (static_cast<size_t>(32) * 64 + 32) * 3;

        // Opaque control.
        glassBox->setTransparency(0.0f);
        RayTracer opaque;
        opaque.begin(cam, rsm, rs);
        int g1 = 0; while (opaque.step(1.0) && g1++ < 1000) {}
        int og = opaque.getPixels()[centerIndex + 1];
        int orr = opaque.getPixels()[centerIndex + 0];
        check(og > orr, "opaque box shows its own green");

        // Fully transparent, no bending.
        glassBox->setTransparency(1.0f);
        RayTracer clear;
        clear.begin(cam, rsm, rs);
        int g2 = 0; while (clear.step(1.0) && g2++ < 1000) {}
        int cr = clear.getPixels()[centerIndex + 0];
        int cg = clear.getPixels()[centerIndex + 1];
        int cb = clear.getPixels()[centerIndex + 2];

        std::printf("        opaque rgb(%d,%d,%d) -> clear rgb(%d,%d,%d)\n",
                    orr, og, opaque.getPixels()[centerIndex + 2], cr, cg, cb);

        check(cr > cg && cr > cb, "IOR 1.0 transmits the backdrop's red through the box");
        check(cb < 60, "background does not leak through the backdrop");
    }

    // ------------------------------------------- interleaved face indices ----
    // ImportShape stores faces as {v0,n0, v1,n1, v2,n2}. Reading those entries
    // as consecutive vertex indices yields scrambled geometry, so the stride
    // must be honoured. Here the vertex and normal index spaces are chosen so a
    // stride-1 misread would hit the WRONG vertices and miss the ray entirely.
    std::printf("\nInterleaved face indices (ImportShape format)\n");
    if (haveIntersector) {
        ShapeManager ism;
        InterleavedShape* mesh = new InterleavedShape(0.0f, 0.0f, 0.0f, 1.0f, 2, 1);

        std::vector<glm::vec3> v;
        v.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));  // 0
        v.push_back(glm::vec3( 1.0f, -1.0f, 0.0f));  // 1
        v.push_back(glm::vec3( 0.0f,  1.0f, 0.0f));  // 2
        v.push_back(glm::vec3(50.0f, 50.0f, 50.0f)); // 3 - far away decoy
        mesh->setVertices(v);

        std::vector<glm::vec3> n;
        n.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
        mesh->setNormals(n);

        // Triangle over vertices 0,1,2 with normal index 0 between each.
        // A stride-1 reader sees {0, 0, 1} - a degenerate triangle - and drops it.
        std::vector<std::vector<int> > f;
        std::vector<int> face;
        face.push_back(0); face.push_back(0);
        face.push_back(1); face.push_back(0);
        face.push_back(2); face.push_back(0);
        f.push_back(face);
        mesh->setFaces(f);
        ism.addShape(mesh);

        RayScene iscene;
        iscene.build(ism);
        check(iscene.triangleCount() == 1, "interleaved face yields exactly 1 triangle");

        Ray ray(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        Hit hit;
        bool got = iscene.intersect(ray, 1e-4f, 1e30f, hit);
        check(got, "ray hits the interleaved triangle");
        check(got && std::fabs(hit.t - 5.0f) < 1e-3f, "hit lands on the z=0 plane");
        check(got && hit.normal.z > 0.99f,
              "explicit normal index is used for the shading normal");
    }

    // ------------------------------------------------- ray cast vs ray trace ----
    // The two modes must produce measurably different images, and ray cast must
    // ignore reflectivity and transparency entirely - no secondary rays at all.
    std::printf("\nRay cast vs ray trace modes\n");
    if (haveIntersector) {
        ShapeManager msm;
        TestShape* shiny = makeBox(0.0f, 0.0f, 0.0f, 1.0f, 2, 1);
        shiny->setReflectivity(0.9f);
        shiny->setTransparency(0.5f);
        msm.addShape(shiny);
        msm.addShape(makeFloor(-1.0f, 4.0f, 7, 2));

        Camera cam;
        cam.setView(3.14159265f / 2.0f, 0.5f, 5.0f);

        RayTraceSettings cast;
        cast.width = 120; cast.height = 90; cast.maxDepth = 5;
        cast.mode = RayTraceSettings::Mode_RayCast;

        RayTraceSettings trace = cast;
        trace.mode = RayTraceSettings::Mode_RayTrace;

        RayTracer a, b;
        a.begin(cam, msm, cast);  int g1 = 0; while (a.step(2.0) && g1++ < 2000) {}
        b.begin(cam, msm, trace); int g2 = 0; while (b.step(2.0) && g2++ < 2000) {}

        long castSum = 0, traceSum = 0;
        for (size_t i = 0; i < a.getPixels().size(); ++i) castSum  += a.getPixels()[i];
        for (size_t i = 0; i < b.getPixels().size(); ++i) traceSum += b.getPixels()[i];
        std::printf("        ray cast sum %ld vs ray trace sum %ld\n", castSum, traceSum);
        check(castSum != traceSum, "the two modes produce different images");

        // Ray cast output must not depend on material recursion settings.
        RayTraceSettings castNoFx = cast;
        castNoFx.reflections = castNoFx.refractions = castNoFx.shadows = false;
        RayTracer c;
        c.begin(cam, msm, castNoFx); int g3 = 0; while (c.step(2.0) && g3++ < 2000) {}

        bool identical = (c.getPixels() == a.getPixels());
        check(identical, "ray cast ignores shadow/reflection/refraction toggles");
    }

    // --------------------------------------------------- lights & materials ----
    std::printf("\nLights as scene objects\n");
    if (haveIntersector) {
        ShapeManager lsm;
        TestShape* target = makeFloor(0.0f, 3.0f, 7, 1);   // flat plate facing +Y
        lsm.addShape(target);

        RayScene s0;
        s0.build(lsm);
        check(s0.getLights().empty(), "no Light objects means no scene lights");

        Light* key = new Light(0.0f, 5.0f, 0.0f, 2);
        lsm.addShape(key);

        RayScene s1;
        s1.build(lsm);
        check(s1.getLights().size() == 1, "a Light is picked up by the scene");
        check(s1.triangleCount() == 2,
              "the light contributes no geometry (plate's 2 triangles only)");

        // A light must never block another light.
        glm::vec3 p(0.0f, 0.001f, 0.0f);
        Ray up(p, glm::vec3(0.0f, 1.0f, 0.0f));
        check(!s1.occluded(up, 1e-4f, 100.0f, 0),
              "a light gizmo does not occlude shadow rays");

        // Disabled lights drop out of the snapshot.
        key->setEnabled(false);
        RayScene s2; s2.build(lsm);
        check(s2.getLights().empty(), "a disabled light is excluded");
        key->setEnabled(true);

        // Two lights must be brighter than one.
        Camera cam;
        cam.setView(3.14159265f / 2.0f, 1.2f, 6.0f);
        RayTraceSettings rs;
        rs.width = 100; rs.height = 80; rs.shadows = false;

        RayTracer one;
        one.begin(cam, lsm, rs); int g1 = 0; while (one.step(2.0) && g1++ < 900) {}
        long oneSum = 0;
        for (size_t i = 0; i < one.getPixels().size(); ++i) oneSum += one.getPixels()[i];

        lsm.addShape(new Light(2.0f, 5.0f, 2.0f, 3));
        RayTracer two;
        two.begin(cam, lsm, rs); int g2 = 0; while (two.step(2.0) && g2++ < 900) {}
        long twoSum = 0;
        for (size_t i = 0; i < two.getPixels().size(); ++i) twoSum += two.getPixels()[i];

        std::printf("        one light %ld -> two lights %ld\n", oneSum, twoSum);
        check(twoSum > oneSum, "a second light makes the scene brighter");
    }

    std::printf("\nMaterials\n");
    {
        Material def;
        check(def.ambient == 1.0f && def.diffuse == 1.0f && def.specular == 1.0f,
              "material coefficients default to 1");
        check(def.shininess == 32.0f,
              "shininess defaults to 32, matching the old hardcoded exponent");
        check(def.reflectivity == 0.0f && def.transparency == 0.0f,
              "surfaces default to opaque and non-reflective");
    }

    // The defaults above are plain data. Seeing kd actually reach the shading
    // maths needs a rendered image, which needs a hit.
    if (haveIntersector) {
        ShapeManager msm;
        TestShape* plate = makeFloor(0.0f, 3.0f, 7, 1);
        msm.addShape(plate);
        msm.addShape(new Light(0.0f, 5.0f, 0.0f, 2));

        Camera cam;
        cam.setView(3.14159265f / 2.0f, 1.2f, 6.0f);
        RayTraceSettings rs;
        rs.width = 100; rs.height = 80; rs.shadows = false;

        RayTracer bright;
        bright.begin(cam, msm, rs); int g1 = 0; while (bright.step(2.0) && g1++ < 900) {}
        long brightSum = 0;
        for (size_t i = 0; i < bright.getPixels().size(); ++i) brightSum += bright.getPixels()[i];

        // Halving kd must visibly darken the surface.
        plate->getMaterial().diffuse = 0.25f;
        RayTracer dim;
        dim.begin(cam, msm, rs); int g2 = 0; while (dim.step(2.0) && g2++ < 900) {}
        long dimSum = 0;
        for (size_t i = 0; i < dim.getPixels().size(); ++i) dimSum += dim.getPixels()[i];

        std::printf("        kd=1.0 %ld -> kd=0.25 %ld\n", brightSum, dimSum);
        check(dimSum < brightSum, "lowering diffuse (kd) darkens the surface");
    }

    // A SKIPPED RUN MUST NOT REPORT SUCCESS.
    //
    // The dependency guard above is there for the student: with the BVH half
    // stubbed it skips the checks that need a hit and names the file to fix,
    // instead of dumping 18 failures into the wrong file. But that leaves nine
    // checks running, none of which can fail -- so the suite used to end with
    //
    //     ALL TESTS PASSED (0 failures)
    //
    // while grading nothing at all. An instructor marking A6's shading half
    // without linking BVH=solution would read that last line and award full
    // marks for an empty shade().
    //
    // So: green is only green when the whole suite ran. Anything else says
    // INCOMPLETE and exits non-zero, which is also what `make check` needs in
    // a student tree where A6 is genuinely unfinished.
    if (!haveIntersector) {
        std::printf("\nINCOMPLETE -- only %d of this suite's checks ran; the rest\n"
                    "were skipped because RayScene::intersect found no geometry.\n"
                    "This is NOT a pass. Fix the BVH half (src/BVH.cpp, graded by\n"
                    "./bvh_test) and run this again.\n", checksRun);
        return 2;
    }

    std::printf("\n%s (%d of %d check%s failed)\n",
                failures == 0 ? "ALL TESTS PASSED" : "TESTS FAILED",
                failures, checksRun, checksRun == 1 ? "" : "s");
    return failures == 0 ? 0 : 1;
}
