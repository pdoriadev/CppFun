// =============================================================================
// Phong local illumination -- the `lighting` topic
// =============================================================================
// See include/Lighting.h for why this is its own translation unit, and for the
// contract with shaders/fragment_shader.glsl. That shader is GIVEN and is the
// reference: these two functions must produce the same numbers it does, or the
// viewport and a ray-traced render of the same scene will disagree.

#include "Lighting.h"

#include <cmath>

LightGeometry lightGeometry(const SceneLightSample& light, const glm::vec3& point) {
    LightGeometry g;

    // TODO(lighting): light direction, distance and distance falloff
    // Fill in `g` for this light as seen from `point`.
    //
    // Two cases. A DIRECTIONAL light (type == 1) has parallel rays: the
    // direction is the same everywhere and there is no falloff, so distance
    // is effectively infinite and attenuation is 1. Note that the stored
    // direction is the way the light travels, and you want the vector from
    // the surface TOWARDS it.
    //
    // A POINT light has a position: direction and distance both come from
    // subtracting, and attenuation is 1 / (constant + linear*d +
    // quadratic*d*d). Guard the zero-distance case rather than normalising
    // a zero vector.
    //
    // Leaving this as the default -- straight up, no falloff -- lights every
    // surface identically from above, which is a recognisable first result
    // rather than a black screen.
    (void)light; (void)point;
    return g;
}

glm::vec3 phongContribution(const SceneLightSample& light,
                            const Material& mat,
                            const glm::vec3& baseColor,
                            const glm::vec3& n,
                            const glm::vec3& viewDir,
                            const glm::vec3& lightDir,
                            float reach) {
    // TODO(lighting): ambient, diffuse and specular for one light
    // Return ambient + diffuse + specular for this one light.
    //
    // Each term is a light colour times a material coefficient -- mat.ambient,
    // mat.diffuse, mat.specular, mat.shininess. Those are the numbers the
    // Materials work puts on a shape; this is where they mean something.
    //
    // Four things to get right:
    //   - ambient ignores `reach`. It stands in for light that arrived by a
    //     path nobody traced, so a shadow ray towards THIS light says nothing
    //     about it. Scaling it makes every shadow a black hole.
    //   - diffuse is Lambert: max(dot(n, lightDir), 0). Clamp at zero, or a
    //     surface facing away subtracts light from the other lights.
    //   - specular is PHONG, not Blinn-Phong: reflect the light direction
    //     about the normal and dot it with the view direction. The GLSL in
    //     shaders/fragment_shader.glsl does it that way and the two must
    //     agree, or the viewport and a render disagree.
    //   - the specular term does NOT take baseColor. A highlight is the
    //     light seen in the surface, which is why a red ball has a white
    //     highlight.
    //
    // Returning just the ambient term is a usable checkpoint: flat, unlit
    // silhouettes in roughly the right colours.
    (void)n; (void)viewDir; (void)lightDir; (void)reach;
    return light.ambient * mat.ambient * baseColor;
}
