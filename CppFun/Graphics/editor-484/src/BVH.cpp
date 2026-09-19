#include "BVH.h"

// fillTriangleHit() reads nothing from Shape, but keeping the include here
// costs nothing and documents that BVH.h only forward-declares it.
#include "Shape.h"

#include <algorithm>
#include <limits>
#include <cmath>

// Everything here is GIVEN. The slab test, Moller-Trumbore, the recursive build
// and the three traversals live in RayIntersect.cpp, which is the `bvh` topic --
// see SOLUTION-LIBRARIES.md and ASSIGNMENTS.md. What is left in this file is
// bookkeeping (AABB accumulation, build() setup) plus fillTriangleHit(), which
// is deliberately given: see the note at the top of RayIntersect.cpp.

// ---------------------------------------------------------------- AABB ----

AABB::AABB()
    : mn( std::numeric_limits<float>::max()),
      mx(-std::numeric_limits<float>::max()) {}

void AABB::expand(const glm::vec3& p) {
    mn = glm::min(mn, p);
    mx = glm::max(mx, p);
}

void AABB::expand(const AABB& b) {
    mn = glm::min(mn, b.mn);
    mx = glm::max(mx, b.mx);
}

glm::vec3 AABB::centroid() const {
    return (mn + mx) * 0.5f;
}

int AABB::largestAxis() const {
    glm::vec3 d = mx - mn;
    if (d.x > d.y && d.x > d.z) return 0;
    return (d.y > d.z) ? 1 : 2;
}

// ------------------------------------------------------------- hit fill ----

void fillTriangleHit(const RayTriangle& tri, int triIndex, const Ray& ray,
                     float t, float u, float v, Hit& out) {
    out.hit      = true;
    out.t        = t;
    out.point    = ray.at(t);
    out.shape    = tri.shape;
    out.triangle = triIndex;

    // Barycentric interpolation of the texture coordinate. intersectTriangle
    // already computed u and v to do the inside test, so this is genuinely two
    // multiplies -- the same reason interpolating the shading normal below is
    // cheap.
    out.hasTexCoords = tri.hasTexCoords;
    if (tri.hasTexCoords) {
        out.uv = tri.t0 * (1.0f - u - v) + tri.t1 * u + tri.t2 * v;
    }

    glm::vec3 faceNormal = glm::cross(tri.v1 - tri.v0, tri.v2 - tri.v0);
    float faceLen = glm::length(faceNormal);
    faceNormal = (faceLen > 1e-12f) ? faceNormal / faceLen
                                    : glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 shading = faceNormal;
    if (tri.hasVertexNormals) {
        glm::vec3 interp = tri.n0 * (1.0f - u - v) + tri.n1 * u + tri.n2 * v;
        if (glm::length(interp) > 1e-12f) shading = glm::normalize(interp);
    }

    // Two normals, two different jobs. Conflating them is what speckles a
    // smooth-shaded transparent mesh.
    //
    // SHADING orientation uses the interpolated normal. Winding order and
    // supplied vertex normals do not always agree - a quad wound clockwise, or
    // an OBJ exported with inconsistent winding, yields a face normal opposite
    // the artist-supplied vertex normals. Deciding this from the face normal
    // flips the shading normal away from the viewer and the surface goes black
    // (this is what the Mobius strip hit).
    const bool shadingFlipped = glm::dot(ray.direction, shading) > 0.0f;

    // SIDEDNESS - am I entering the solid or leaving it - uses the triangle
    // plane instead. Whether a ray is inside an object is a fact about its
    // geometry, not about the normals someone baked into the file. On a smooth
    // mesh the interpolated normal bends away from the face by design, so near
    // silhouettes and in tight concavities it reports the wrong side;
    // refraction then picks the inverted IOR ratio and scatters that ray
    // somewhere arbitrary. Measured on a bumpy torus at maxDepth 8: 431
    // speckled pixels deciding from the shading normal, 53 deciding from the
    // face.
    out.geoNormal = faceNormal;
    out.backface  = glm::dot(ray.direction, faceNormal) > 0.0f;
    out.normal    = shadingFlipped ? -shading : shading;
}

// ----------------------------------------------------------------- BVH ----

BVH::BVH() {}

void BVH::clear() {
    nodes.clear();
    indices.clear();
}

void BVH::build(const std::vector<RayTriangle>& tris) {
    clear();
    if (tris.empty()) return;

    indices.resize(tris.size());
    for (size_t i = 0; i < tris.size(); ++i) indices[i] = static_cast<int>(i);

    nodes.reserve(tris.size() * 2);
    buildRecursive(tris, 0, static_cast<int>(tris.size()), 0);
}
