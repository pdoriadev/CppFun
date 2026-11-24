#ifndef BVH_H
#define BVH_H

#include <glm/glm.hpp>
#include <vector>
#include "Ray.h"

class Shape;

// One world-space triangle. Vertices are pre-transformed at build time, so
// intersection needs no per-ray matrix work.
struct RayTriangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 n0, n1, n2;      // vertex normals; zero-length means "use face normal"
    glm::vec2 t0, t1, t2;      // texture coordinates; see hasTexCoords
    const Shape* shape;
    bool hasVertexNormals;
    bool hasTexCoords;         // false means the shape has no parameterisation

    RayTriangle() : v0(0.0f), v1(0.0f), v2(0.0f),
                    n0(0.0f), n1(0.0f), n2(0.0f),
                    t0(0.0f), t1(0.0f), t2(0.0f),
                    shape(0), hasVertexNormals(false), hasTexCoords(false) {}
};

struct AABB {
    glm::vec3 mn;
    glm::vec3 mx;

    AABB();
    void expand(const glm::vec3& p);
    void expand(const AABB& b);
    glm::vec3 centroid() const;
    int largestAxis() const;

    // Slab test. Returns true and narrows [tMin,tMax] if the ray hits the box.
    bool intersect(const Ray& r, const glm::vec3& invDir, float tMin, float tMax) const;
};

// Bounding volume hierarchy over a triangle array, split at the centroid median
// of the widest axis. Build is O(n log n) and fast enough to rebuild whenever
// the scene changes, which keeps correctness simple: shapes move constantly in
// this editor, so a stale hierarchy would be worse than a cheap rebuild.
class BVH {
public:
    BVH();

    void build(const std::vector<RayTriangle>& tris);
    void clear();

    // Nearest hit.
    bool intersect(const std::vector<RayTriangle>& tris,
                   const Ray& ray, float tMin, float tMax, Hit& out) const;

    // Any hit - stops at the first blocker. Used for shadow rays.
    bool occluded(const std::vector<RayTriangle>& tris,
                  const Ray& ray, float tMin, float tMax,
                  const Shape* ignore) const;

    // How much of a light survives the trip along this ray: 1 = clear line of
    // sight, 0 = fully blocked, in between = it passed through transparent
    // surfaces. occluded() cannot answer this because it stops at the first
    // triangle, which makes a sheet of glass shadow exactly like a brick.
    float transmittance(const std::vector<RayTriangle>& tris,
                        const Ray& ray, float tMin, float tMax,
                        const Shape* ignore) const;

    size_t nodeCount() const { return nodes.size(); }
    bool empty() const { return nodes.empty(); }

private:
    struct Node {
        AABB box;
        int  start;   // index into `indices`, leaves only
        int  count;   // 0 for interior nodes
        int  left;
        int  right;
        Node() : start(0), count(0), left(-1), right(-1) {}
    };

    std::vector<Node> nodes;
    std::vector<int>  indices;

    int buildRecursive(const std::vector<RayTriangle>& tris, int start, int count, int depth);
};

// Moller-Trumbore. Does not cull backfaces: refraction needs to hit the far
// side of a closed mesh from the inside.
bool intersectTriangle(const RayTriangle& tri, const Ray& ray,
                       float tMin, float tMax,
                       float& tOut, float& uOut, float& vOut);

// Fills `out` from a triangle that has already been intersected, given the
// distance and the barycentric coordinates intersectTriangle returned.
//
// Given, and called from BVH::intersect. It interpolates the shading normal and
// the texture coordinate, and -- the part that matters -- keeps the shading
// normal and the geometric one apart: one decides which way the surface faces
// the light, the other decides whether the ray is entering or leaving a solid.
// Conflating them speckles smooth-shaded transparent meshes.
void fillTriangleHit(const RayTriangle& tri, int triIndex, const Ray& ray,
                     float t, float u, float v, Hit& out);

#endif // BVH_H
