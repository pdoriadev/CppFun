// =============================================================================
// Ray intersection and BVH traversal
// =============================================================================
// Split out of BVH.cpp for the same reason Shading.cpp was split out of
// RayTracer.cpp: the unit the linker can swap is a whole translation unit, so
// the student's work has to live in a file of its own. See SOLUTION-LIBRARIES.md
// and ASSIGNMENTS.md.
//
// BVH.cpp keeps everything that stays given -- the AABB bookkeeping, build()'s
// setup, and fillTriangleHit(), which populates a Hit once a triangle has been
// intersected. This file holds the six functions that ARE the assignment:
//
//   AABB::intersect       the slab test
//   intersectTriangle     Moller-Trumbore
//   BVH::buildRecursive   median split on the widest centroid axis
//   BVH::intersect        traversal for the nearest hit
//   BVH::occluded         traversal that stops at the first blocker
//   BVH::transmittance    traversal that accumulates through transparent hits
//
// All six move together on purpose. The three traversals are the same loop with
// three different stopping rules, so leaving any one of them in the repo hands
// over the other two.
//
// A note on why fillTriangleHit is NOT part of this: deciding which normal to
// use for shading and which for sidedness is a genuinely subtle piece of
// Whitted-tracer lore that took a bug hunt to get right, and it belongs to the
// shading assignment's territory rather than to "walk a tree and intersect a
// triangle". Students calling it get a correct Hit the moment their traversal
// finds the right triangle, which keeps the failure they are debugging squarely
// inside their own code.

#include "BVH.h"

// transmittance() asks each hit shape for its transparency, so the full
// definition is needed here - BVH.h only forward-declares Shape.
#include "Shape.h"

#include <algorithm>
#include <limits>
#include <cmath>

// ---------------------------------------------------------------- AABB ----

bool AABB::intersect(const Ray& r, const glm::vec3& invDir, float tMin, float tMax) const {
    // TODO(bvh): the slab test -- does this ray cross this box
    // For each of the three axes, find where the ray crosses the two planes
    // of that slab, keep the running [tMin, tMax] overlap, and reject as soon
    // as the interval closes. invDir is 1/direction, precomputed by the
    // caller so this stays division-free.
    //
    // Watch the degenerate case: a flat box (a floor, any planar mesh) has
    // zero extent on one axis, so tMin == tMax exactly on that slab.
    (void)r; (void)invDir; (void)tMin; (void)tMax;
    return true;   // "everything hits every box" -- correct but useless: the
                   // tree still returns right answers, just at brute-force speed
}

// ------------------------------------------------------------ triangle ----

bool intersectTriangle(const RayTriangle& tri, const Ray& ray,
                       float tMin, float tMax,
                       float& tOut, float& uOut, float& vOut) {
    // TODO(bvh): Moller-Trumbore ray/triangle intersection
    // Moller-Trumbore. Return false unless the ray meets the triangle's
    // plane within [tMin, tMax] AND lands inside the triangle; on a hit write
    // the distance to tOut and the barycentric coordinates to uOut and vOut.
    //
    // Those barycentrics are not bookkeeping -- fillTriangleHit uses them to
    // interpolate the shading normal and the texture coordinate, so a hit
    // reported with wrong u,v renders as flat shading and smeared textures
    // rather than as a missing triangle.
    //
    // Do NOT cull backfaces: refraction has to hit the far side of a closed
    // mesh from the inside, so a negative determinant is a legitimate hit.
    (void)tri; (void)ray; (void)tMin; (void)tMax;
    (void)tOut; (void)uOut; (void)vOut;
    return false;
}

// ----------------------------------------------------------------- BVH ----

int BVH::buildRecursive(const std::vector<RayTriangle>& tris, int start, int count, int depth) {
    // TODO(bvh): build one node -- bound it, decide leaf or split, recurse
    // Build the subtree covering indices[start .. start+count) and return the
    // index of the node you created.
    //
    // Push a Node FIRST and remember its index: the recursive calls push
    // their own nodes, so `nodes` reallocates underneath you. Holding a
    // Node& across a recursive call is the classic way to break this.
    //
    // Bound the range, then either stop (a leaf records start and count) or
    // partition it and recurse. std::nth_element on `indices`, keyed on the
    // triangle centroid along the widest axis of the CENTROID bounds, gives
    // a median split in O(n) without sorting.
    //
    // Two stopping rules you need beyond "few enough triangles": a depth cap,
    // and coincident centroids -- an unsplittable set recurses forever
    // because every split puts all the triangles back on one side.
    //
    // Interior nodes must set count = 0; traversal uses count > 0 to mean
    // "leaf".
    //
    // Returning one big leaf (start, count, no recursion) is a legal BVH and
    // renders correctly -- just at brute-force speed. Start there, confirm
    // the image is right, then make it a tree.
    (void)tris; (void)depth;
    int nodeIndex = static_cast<int>(nodes.size());
    nodes.push_back(Node());
    nodes[nodeIndex].start = start;
    nodes[nodeIndex].count = count;
    return nodeIndex;
}

bool BVH::intersect(const std::vector<RayTriangle>& tris,
                    const Ray& ray, float tMin, float tMax, Hit& out) const {
    // TODO(bvh): traverse for the NEAREST hit
    // Walk the tree from the root and leave `out` describing the CLOSEST
    // triangle hit in [tMin, tMax]; return whether there was one.
    //
    // An explicit stack of node indices rather than recursion: this runs once
    // per pixel per bounce, and the call overhead shows up in the frame time.
    //
    // Test a node's box before opening it, and test it against the nearest
    // hit SO FAR, not against tMax -- that shrinking interval is most of what
    // the tree buys you. count > 0 means a leaf: intersect its triangles.
    // Otherwise push the children.
    //
    // On a hit that beats the current nearest, call fillTriangleHit(); it is
    // given, and handles the normal and texture-coordinate interpolation.
    //
    // Precompute invDir = 1/direction once per ray -- AABB::intersect expects
    // it, and recomputing per node puts three divisions in the hot loop.
    (void)tris; (void)ray; (void)tMin; (void)tMax; (void)out;
    return false;
}

bool BVH::occluded(const std::vector<RayTriangle>& tris,
                   const Ray& ray, float tMin, float tMax,
                   const Shape* ignore) const {
    // TODO(bvh): traverse for ANY hit -- shadow rays
    // Same walk as intersect(), one stopping rule different: a shadow ray only
    // asks "is anything in the way", so return true at the FIRST hit and stop
    // walking. Nothing shrinks here -- there is no nearest hit to track.
    //
    // `ignore` is the shape the ray started from; skip its triangles. Without
    // that, a surface shadows itself and everything goes black.
    (void)tris; (void)ray; (void)tMin; (void)tMax; (void)ignore;
    return false;   // nothing occludes anything: the scene renders with no
                    // shadows at all, which is a useful first checkpoint
}

float BVH::transmittance(const std::vector<RayTriangle>& tris,
                         const Ray& ray, float tMin, float tMax,
                         const Shape* ignore) const {
    // TODO(bvh): traverse accumulating through transparent surfaces
    // The third stopping rule. occluded() cannot answer this question: it
    // stops at the first triangle, which makes a sheet of glass shadow exactly
    // like a brick.
    //
    // Walk the whole ray, and for every triangle actually hit multiply a
    // running factor by that shape's getTransparency(). Return 1 for a clear
    // line of sight, 0 once anything opaque is in the way.
    //
    // Each surface crossed takes its cut, so a solid block contributes twice
    // -- once entering, once leaving. That is wanted, not a bug: a thick
    // object should shadow more than a pane of glass.
    //
    // Two early exits keep this affordable: an opaque hit ends it outright,
    // and so does a factor small enough to be indistinguishable from black.
    //
    // Returning 1.0f is the no-shadow checkpoint; returning
    // occluded(...) ? 0.0f : 1.0f gives you hard shadows to build on.
    (void)tris; (void)ray; (void)tMin; (void)tMax; (void)ignore;
    return 1.0f;
}
