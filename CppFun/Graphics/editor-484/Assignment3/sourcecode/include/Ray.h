#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <limits>

class Shape;

// A ray in world space. `direction` is expected to be normalized.
struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray() : origin(0.0f), direction(0.0f, 0.0f, -1.0f) {}
    Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(d) {}

    glm::vec3 at(float t) const { return origin + direction * t; }
};

// Result of a ray/scene query.
struct Hit {
    bool         hit;
    float        t;
    glm::vec3    point;
    glm::vec3    normal;    // world space, already flipped to oppose the ray
    glm::vec3    geoNormal; // unflipped face normal, needed for refraction sidedness
    const Shape* shape;
    int          triangle;
    bool         backface;
    // Interpolated texture coordinate at the hit, valid only when
    // hasTexCoords is set. intersectTriangle already returns the barycentric
    // u and v, so this costs two multiplies -- the expensive part of texturing
    // in a tracer is the sample, not getting to it.
    glm::vec2    uv;
    bool         hasTexCoords;

    Hit()
        : hit(false),
          t(std::numeric_limits<float>::max()),
          point(0.0f),
          normal(0.0f, 1.0f, 0.0f),
          geoNormal(0.0f, 1.0f, 0.0f),
          shape(0),
          triangle(-1),
          backface(false),
          uv(0.0f),
          hasTexCoords(false) {}
};

#endif // RAY_H
