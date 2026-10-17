// Grader for the `bvh` topic: ray/box, ray/triangle, the tree build, and the
// three traversals.
//
// Deliberately GL-free and scene-free. Everything here is built from literal
// triangles, so it links against almost nothing and runs in milliseconds --
// which matters because this is the suite a student runs on every edit while
// their traversal is still returning nothing.
//
// The through-line: each traversal is checked against the answer computed by
// brute force over the same triangles. A BVH is an optimisation, so the only
// correctness question it can be asked is "does it agree with looking at
// everything", and phrasing the assertions that way means the suite cannot be
// satisfied by a tree that merely returns *a* hit.

#include "BVH.h"
#include "Shape.h"

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

// A Shape with no geometry of its own. transmittance() and occluded() both take
// a shape pointer to ignore, and transmittance() asks whatever it hits for its
// transparency, so the triangles need to belong to something -- but nothing here
// draws, so a bare subclass is enough (the same trick transform_test uses).
class TestShape : public Shape {
public:
    TestShape(int id) : Shape(0.0f, 0.0f, 0.0f, 1.0f, 0, id) { shapeType = "Test"; }
    void draw(GLuint) override {}
};

static int checks = 0, failures = 0;

static void ok(bool cond, const std::string& what) {
    ++checks;
    if (cond) {
        std::printf("  PASS  %s\n", what.c_str());
    } else {
        std::printf("  FAIL  %s\n", what.c_str());
        ++failures;
    }
}

// ---------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------

static RayTriangle tri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    RayTriangle t;
    t.v0 = a; t.v1 = b; t.v2 = c;
    return t;
}

// An axis-aligned quad at depth z, spanning -s..s in x and y, as two triangles.
static void addQuad(std::vector<RayTriangle>& out, float z, float s,
                    const Shape* owner = 0) {
    out.push_back(tri(glm::vec3(-s, -s, z), glm::vec3(s, -s, z), glm::vec3(s, s, z)));
    out.push_back(tri(glm::vec3(-s, -s, z), glm::vec3(s, s, z), glm::vec3(-s, s, z)));
    out[out.size() - 2].shape = owner;
    out[out.size() - 1].shape = owner;
}

// Deterministic pseudo-random scatter: a real tree with real splits, identical
// on every machine and every run, so a failure is reproducible.
static unsigned seed = 12345u;
static float rnd(float lo, float hi) {
    seed = seed * 1103515245u + 12345u;
    const float f = ((seed >> 16) & 0x7fff) / 32767.0f;
    return lo + f * (hi - lo);
}

static std::vector<RayTriangle> scatter(int n) {
    std::vector<RayTriangle> out;
    for (int i = 0; i < n; ++i) {
        const glm::vec3 c(rnd(-6.0f, 6.0f), rnd(-6.0f, 6.0f), rnd(-6.0f, 6.0f));
        out.push_back(tri(c,
                          c + glm::vec3(rnd(0.1f, 0.9f), rnd(-0.4f, 0.4f), rnd(-0.4f, 0.4f)),
                          c + glm::vec3(rnd(-0.4f, 0.4f), rnd(0.1f, 0.9f), rnd(-0.4f, 0.4f))));
    }
    return out;
}

// The reference every traversal check is measured against.
static bool bruteForce(const std::vector<RayTriangle>& tris, const Ray& ray,
                       float tMin, float tMax, float& tOut, int& which) {
    bool found = false;
    which = -1;
    float best = tMax;
    for (size_t i = 0; i < tris.size(); ++i) {
        float t, u, v;
        if (intersectTriangle(tris[i], ray, tMin, best, t, u, v)) {
            best = t;
            which = static_cast<int>(i);
            found = true;
        }
    }
    tOut = best;
    return found;
}

int main() {
    std::printf("BVH and ray intersection\n");

    // --- ray/box ------------------------------------------------------------
    {
        AABB box;
        box.expand(glm::vec3(-1.0f));
        box.expand(glm::vec3( 1.0f));

        const glm::vec3 dir(0.0f, 0.0f, -1.0f);
        Ray through(glm::vec3(0.0f, 0.0f, 5.0f), dir);
        const glm::vec3 inv(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);

        ok(box.intersect(through, inv, 0.0f, 1e30f), "a ray through the box hits it");

        Ray past(glm::vec3(3.0f, 3.0f, 5.0f), dir);
        ok(!box.intersect(past, inv, 0.0f, 1e30f), "a ray beside the box misses");

        // The interval matters as much as the geometry: traversal narrows tMax
        // to the nearest hit so far, and a slab test that ignores it opens every
        // node in the scene.
        ok(!box.intersect(through, inv, 0.0f, 1.0f),
           "a box beyond tMax is rejected -- traversal relies on this to prune");

        Ray inside(glm::vec3(0.0f), dir);
        ok(box.intersect(inside, inv, 0.0f, 1e30f), "a ray starting inside hits");

        // The '<' vs '<=' trap. A flat box has zero extent on one axis, so
        // tMin == tMax exactly on that slab; rejecting on equality makes every
        // floor, wall and planar mesh invisible.
        AABB flat;
        flat.expand(glm::vec3(-1.0f, 0.0f, -1.0f));
        flat.expand(glm::vec3( 1.0f, 0.0f,  1.0f));
        const glm::vec3 down(0.0f, -1.0f, 0.0f);
        Ray onto(glm::vec3(0.0f, 5.0f, 0.0f), down);
        const glm::vec3 invDown(1.0f / down.x, 1.0f / down.y, 1.0f / down.z);
        ok(flat.intersect(onto, invDown, 0.0f, 1e30f),
           "a ZERO-THICKNESS box is still hit -- the slab test must not reject "
           "on tMin == tMax, or every floor in the scene disappears");
    }

    // --- ray/triangle -------------------------------------------------------
    {
        RayTriangle t = tri(glm::vec3(-1.0f, -1.0f, 0.0f),
                            glm::vec3( 1.0f, -1.0f, 0.0f),
                            glm::vec3(-1.0f,  1.0f, 0.0f));

        float tt, u, v;
        Ray centre(glm::vec3(-0.5f, -0.5f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ok(intersectTriangle(t, centre, 1e-4f, 1e30f, tt, u, v), "a ray through it hits");
        ok(std::fabs(tt - 4.0f) < 1e-4f, "at the right distance");

        // Barycentrics are load-bearing, not bookkeeping: fillTriangleHit
        // interpolates the shading normal and the texture coordinate from them,
        // so wrong u,v renders as flat shading and smeared textures rather than
        // as a missing triangle.
        {
            float t2, u2, v2;
            Ray atV0(glm::vec3(-1.0f, -1.0f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f));
            intersectTriangle(t, atV0, 1e-4f, 1e30f, t2, u2, v2);
            ok(std::fabs(u2) < 1e-3f && std::fabs(v2) < 1e-3f,
               "the barycentrics are 0,0 at the first vertex");

            Ray atV1(glm::vec3(1.0f, -1.0f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f));
            intersectTriangle(t, atV1, 1e-4f, 1e30f, t2, u2, v2);
            ok(std::fabs(u2 - 1.0f) < 1e-3f && std::fabs(v2) < 1e-3f,
               "and 1,0 at the second -- u,v are what the shading normal and "
               "the texture coordinate get interpolated from");
        }

        Ray outside(glm::vec3(0.9f, 0.9f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ok(!intersectTriangle(t, outside, 1e-4f, 1e30f, tt, u, v),
           "a ray through the triangle's PLANE but outside its edges misses");

        Ray parallel(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ok(!intersectTriangle(t, parallel, 1e-4f, 1e30f, tt, u, v),
           "a ray parallel to the plane misses rather than dividing by zero");

        Ray behind(glm::vec3(-0.5f, -0.5f, -4.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ok(!intersectTriangle(t, behind, 1e-4f, 1e30f, tt, u, v),
           "a triangle behind the ray is not hit");

        // Backfaces must NOT be culled: refraction hits the far side of a closed
        // mesh from the inside, and culling there loses every exit ray.
        Ray fromBehind(glm::vec3(-0.5f, -0.5f, -4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ok(intersectTriangle(t, fromBehind, 1e-4f, 1e30f, tt, u, v),
           "the BACK face is hit too -- culling it breaks refraction, which has "
           "to leave a solid through its far side");
    }

    // --- the build ----------------------------------------------------------
    {
        BVH empty;
        std::vector<RayTriangle> none;
        empty.build(none);
        ok(empty.empty(), "an empty triangle list builds an empty tree");

        Hit h;
        Ray r(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ok(!empty.intersect(none, r, 1e-4f, 1e30f, h),
           "and tracing it hits nothing rather than reading node 0");

        std::vector<RayTriangle> many = scatter(400);
        BVH bvh;
        bvh.build(many);
        ok(!bvh.empty(), "400 triangles build a tree");

        // A tree, not one big leaf. Correct-but-linear is a legitimate first
        // checkpoint and this is the check that says you are no longer there.
        ok(bvh.nodeCount() > 1,
           "the tree actually splits -- a single node is a correct BVH and a "
           "brute-force search, which is the checkpoint before this one");
        ok(bvh.nodeCount() < many.size() * 2 + 4,
           "and it does not create more nodes than a binary tree over the "
           "triangles can have");

        // Coincident centroids: every split puts all the triangles on one side,
        // so a build without this guard recurses until it runs out of stack.
        std::vector<RayTriangle> stacked;
        for (int i = 0; i < 64; ++i) {
            stacked.push_back(tri(glm::vec3(-1.0f, -1.0f, 0.0f),
                                  glm::vec3( 1.0f, -1.0f, 0.0f),
                                  glm::vec3(-1.0f,  1.0f, 0.0f)));
        }
        BVH degenerate;
        degenerate.build(stacked);
        ok(!degenerate.empty(),
           "64 IDENTICAL triangles build without recursing forever -- an "
           "unsplittable set has to become a leaf");
    }

    // --- nearest hit --------------------------------------------------------
    {
        std::vector<RayTriangle> tris;
        addQuad(tris, 0.0f, 2.0f);      // near
        addQuad(tris, -3.0f, 2.0f);     // far, directly behind it

        BVH bvh;
        bvh.build(tris);

        Ray r(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        Hit h;
        ok(bvh.intersect(tris, r, 1e-4f, 1e30f, h), "a ray at two stacked quads hits");
        ok(std::fabs(h.t - 5.0f) < 1e-3f,
           "and returns the NEAR one -- traversal must keep the closest hit, "
           "not the first one it happens to walk into");

        ok(h.normal.z > 0.5f,
           "the hit carries a normal facing the ray, so fillTriangleHit ran");
        ok(h.point.z > -0.1f && h.point.z < 0.1f, "and the hit point is on the surface");

        // tMin/tMax are the interface refraction and shadow rays rely on.
        ok(!bvh.intersect(tris, r, 1e-4f, 1.0f, h),
           "nothing within tMax = 1 is hit");
        ok(bvh.intersect(tris, r, 6.0f, 1e30f, h) && std::fabs(h.t - 8.0f) < 1e-3f,
           "raising tMin past the near quad returns the far one");
    }

    // --- agreement with brute force -----------------------------------------
    //
    // The check that matters. A BVH is an optimisation, so the question is not
    // "does it find something" but "does it find exactly what looking at every
    // triangle would find" -- over enough rays to reach the awkward ones that
    // graze a box corner or land on a split plane.
    {
        std::vector<RayTriangle> tris = scatter(300);
        BVH bvh;
        bvh.build(tris);

        int agreed = 0, disagreed = 0, mismatchedT = 0;
        const int kRays = 500;
        for (int i = 0; i < kRays; ++i) {
            const glm::vec3 origin(rnd(-9.0f, 9.0f), rnd(-9.0f, 9.0f), 12.0f);
            glm::vec3 dir(rnd(-0.35f, 0.35f), rnd(-0.35f, 0.35f), -1.0f);
            dir = glm::normalize(dir);
            Ray r(origin, dir);

            float refT; int refIdx;
            const bool refHit = bruteForce(tris, r, 1e-4f, 1e30f, refT, refIdx);

            Hit h;
            const bool bvhHit = bvh.intersect(tris, r, 1e-4f, 1e30f, h);

            if (refHit != bvhHit) { ++disagreed; continue; }
            if (refHit && std::fabs(h.t - refT) > 1e-3f) { ++mismatchedT; continue; }
            ++agreed;
        }

        std::printf("  %d/%d rays agree with brute force (%d hit/miss, %d distance)\n",
                    agreed, kRays, disagreed, mismatchedT);
        ok(disagreed == 0,
           "the tree and brute force agree on WHETHER every ray hits -- a "
           "disagreement here is a box test rejecting a node it should open");
        ok(mismatchedT == 0,
           "and on the distance -- a mismatch here is traversal returning a "
           "hit that is not the nearest one");
    }

    // --- occlusion ----------------------------------------------------------
    {
        std::vector<RayTriangle> tris = scatter(300);
        BVH bvh;
        bvh.build(tris);

        int wrong = 0;
        const int kRays = 300;
        for (int i = 0; i < kRays; ++i) {
            const glm::vec3 origin(rnd(-9.0f, 9.0f), rnd(-9.0f, 9.0f), 12.0f);
            glm::vec3 dir(rnd(-0.35f, 0.35f), rnd(-0.35f, 0.35f), -1.0f);
            dir = glm::normalize(dir);
            Ray r(origin, dir);

            float refT; int refIdx;
            const bool refHit = bruteForce(tris, r, 1e-4f, 1e30f, refT, refIdx);
            if (bvh.occluded(tris, r, 1e-4f, 1e30f, 0) != refHit) ++wrong;
        }
        ok(wrong == 0,
           "occluded() answers the same yes/no as brute force on 300 rays -- "
           "it may stop early, but not sooner than the first real blocker");

        // The self-shadowing guard. Without `ignore`, every surface shadows
        // itself and the scene renders black.
        TestShape owner(1);
        std::vector<RayTriangle> quad;
        addQuad(quad, 0.0f, 2.0f, &owner);
        BVH one;
        one.build(quad);

        Ray r(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ok(one.occluded(quad, r, 1e-4f, 1e30f, 0), "a quad in the way occludes");
        ok(!one.occluded(quad, r, 1e-4f, 1e30f, &owner),
           "and does not once its own shape is ignored -- this is what stops a "
           "surface shadowing itself");
    }

    // --- transmittance ------------------------------------------------------
    //
    // The one traversal that returns something other than a boolean: it walks
    // the whole ray and multiplies a factor by every surface it crosses, which
    // is why occluded() cannot answer this question -- occluded() stops at the
    // first triangle, so a sheet of glass would shadow exactly like a brick.
    {
        TestShape owner(2);
        std::vector<RayTriangle> quad;
        addQuad(quad, 0.0f, 2.0f, &owner);
        BVH bvh;
        bvh.build(quad);

        // Off the quad's centre on purpose: the two triangles share the
        // diagonal from (-2,-2) to (2,2), and a ray down the middle lands
        // exactly on it and is counted by BOTH -- which halves the answer twice
        // and looks like a traversal bug when it is a fixture bug.
        Ray blocked(glm::vec3(0.5f, -0.5f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ok(bvh.transmittance(quad, blocked, 1e-4f, 1e30f, 0) == 0.0f,
           "an opaque surface transmits nothing");

        Ray clear(glm::vec3(8.0f, 8.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ok(bvh.transmittance(quad, clear, 1e-4f, 1e30f, 0) == 1.0f,
           "a clear line of sight transmits everything");

        ok(bvh.transmittance(quad, blocked, 1e-4f, 1e30f, &owner) == 1.0f,
           "and the ignored shape does not block its own light");

        // Half-transparent, hit once: the factor is that surface's own
        // transparency. A traversal that stops at the first hit cannot produce
        // this number at all.
        owner.setTransparency(0.5f);
        const float half = bvh.transmittance(quad, blocked, 1e-4f, 1e30f, 0);
        ok(std::fabs(half - 0.5f) < 1e-4f,
           "a half-transparent surface passes half the light through");

        // Two sheets in a row multiply rather than replace, which is what makes
        // a solid object -- entered and left, so crossed twice -- shadow more
        // than a single pane.
        TestShape second(3);
        second.setTransparency(0.5f);
        std::vector<RayTriangle> two;
        addQuad(two, 0.0f, 2.0f, &owner);
        addQuad(two, -3.0f, 2.0f, &second);
        BVH pair;
        pair.build(two);
        const float both = pair.transmittance(two, blocked, 1e-4f, 1e30f, 0);
        ok(std::fabs(both - 0.25f) < 1e-4f,
           "two half-transparent surfaces multiply to a quarter -- every surface "
           "crossed takes its cut, so a solid shadows more than a pane of glass");

        std::vector<RayTriangle> none;
        BVH empty;
        empty.build(none);
        ok(empty.transmittance(none, blocked, 1e-4f, 1e30f, 0) == 1.0f,
           "an empty scene transmits everything rather than reading node 0");
    }

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) { std::printf("ALL TESTS PASSED (0 failures)\n"); return 0; }
    std::printf("TESTS FAILED (%d failures)\n", failures);
    return 1;
}
