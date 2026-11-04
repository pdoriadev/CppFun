#ifndef LIGHTING_H
#define LIGHTING_H

#include <glm/glm.hpp>

#include "Material.h"
#include "RayScene.h"     // SceneLightSample

// =============================================================================
// The local illumination model
// =============================================================================
// Phong: ambient + diffuse + specular, per light, with distance attenuation.
//
// Split out of Shading.cpp so it is its own translation unit and its own topic.
// The division is deliberate and is the same one the bvh/raytracer split makes:
//
//   lighting     what colour is this surface under this light
//   raytracer    which rays to cast, and how to combine what they bring back
//
// A student can get one working without the other, and they fail differently:
// broken lighting gives a correctly-shaped image in the wrong colours, broken
// recursion gives correct colours with no reflections or shadows. Told apart
// only if they can be swapped independently.
//
// WHY THIS BELONGS WITH MATERIALS. Every term here is a material coefficient
// times a light colour. `mat.ambient`, `mat.diffuse`, `mat.specular` and
// `mat.shininess` are exactly what the Materials assignment puts on a shape;
// this is where those numbers finally mean something. See ASSIGNMENTS.md --
// it is A5's second half for 484, and folded into A6 for 566, but it is a topic
// of its own so it can be promoted to a standalone assignment without touching
// any code.
//
// THE CONTRACT THAT MATTERS: this must agree with the GLSL in
// shaders/fragment_shader.glsl, term for term. The rasterizer and the ray
// tracer draw the same scene, and a student who changes one without the other
// gets a viewport and a render that disagree -- which is a confusing bug to
// chase and an easy one to cause. The shader is given and is the reference;
// match it.

// Where a light is, as seen from a point on a surface.
//
// Directional lights have no position: their rays are parallel, so the
// direction is constant and there is no falloff however far away the surface
// is. Point lights have both. Keeping the two cases in one function is what
// stops the difference being rediscovered at every call site.
struct LightGeometry {
    glm::vec3 direction;    // unit vector, surface -> light
    float     distance;     // 1e30f for a directional light
    float     attenuation;  // 1.0f for a directional light

    LightGeometry()
        : direction(0.0f, 1.0f, 0.0f), distance(1e30f), attenuation(1.0f) {}
};

LightGeometry lightGeometry(const SceneLightSample& light, const glm::vec3& point);

// The Phong terms for ONE light at one point.
//
//   n         shading normal, already oriented towards the viewer
//   viewDir   unit vector, surface -> eye
//   lightDir  unit vector, surface -> light (from lightGeometry above)
//   baseColor the surface colour, texture already multiplied in
//   reach     how much of this light survives the trip to the surface:
//             1 clear, 0 fully shadowed, in between through something
//             transparent. Ambient ignores it -- ambient is the standing-in
//             for light that arrived by some path nobody traced, and a shadow
//             ray says nothing about that.
//
// Attenuation is NOT applied here. The caller multiplies it into the total,
// which is what the shader does, and it keeps this function a pure statement of
// the reflectance model.
glm::vec3 phongContribution(const SceneLightSample& light,
                            const Material& mat,
                            const glm::vec3& baseColor,
                            const glm::vec3& n,
                            const glm::vec3& viewDir,
                            const glm::vec3& lightDir,
                            float reach);

#endif // LIGHTING_H
