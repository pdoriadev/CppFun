#ifndef RAYSCENE_H
#define RAYSCENE_H

#include <vector>
#include "BVH.h"
#include "Ray.h"

class ShapeManager;

// A light flattened to what shading actually needs, captured when the scene is
// built so a render is not affected by edits made while it runs.
struct SceneLightSample {
    int       type;          // matches Light::LightType
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float     constant;
    float     linear;
    float     quadratic;

    SceneLightSample()
        : type(0), position(0.0f), direction(0.0f, -1.0f, 0.0f),
          ambient(0.0f), diffuse(0.0f), specular(0.0f),
          constant(1.0f), linear(0.0f), quadratic(0.0f) {}
};

// A snapshot of the editor scene flattened into world-space triangles plus a
// BVH over them.
//
// Everything in this editor derives from Shape and carries vertices/faces, so
// one triangle intersector covers cubes, spheres, teapots, imported OBJ meshes
// and surfaces alike - no per-primitive analytic intersection needed.
//
// Shapes that render from their own GL buffers without populating the shared
// vertex/face arrays (particle systems, for example) contribute no geometry and
// are simply skipped.
class RayScene {
public:
    RayScene();

    // Flattens the current scene. Call whenever geometry or transforms change.
    void build(ShapeManager& shapeManager);
    void clear();

    bool intersect(const Ray& ray, float tMin, float tMax, Hit& out) const;
    bool occluded(const Ray& ray, float tMin, float tMax, const Shape* ignore) const;

    // Fraction of a light that reaches the far end of the ray, accounting for
    // transparent surfaces along the way. 1 = clear, 0 = blocked.
    float transmittance(const Ray& ray, float tMin, float tMax,
                        const Shape* ignore) const;

    const std::vector<SceneLightSample>& getLights() const { return lights; }

    size_t triangleCount() const { return triangles.size(); }
    size_t shapeCount() const { return shapesIncluded; }
    bool empty() const { return triangles.empty(); }

private:
    std::vector<RayTriangle> triangles;
    std::vector<SceneLightSample> lights;
    BVH bvh;
    size_t shapesIncluded;
};

#endif // RAYSCENE_H
