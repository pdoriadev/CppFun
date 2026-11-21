// =============================================================================
// Phong local illumination -- the `lighting` topic
// =============================================================================
// See include/Lighting.h for why this is its own translation unit, and for the
// contract with shaders/fragment_shader.glsl. That shader is GIVEN and is the
// reference: these two functions must produce the same numbers it does, or the
// viewport and a ray-traced render of the same scene will disagree.

#include "../include/Lighting.h"

#include <iostream>
#include <cmath>

LightGeometry lightGeometry(const SceneLightSample& light, const glm::vec3& point) {
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

    LightGeometry g;
    if (light.type != 0 && light.type != 1) {
        std::cout << "\n" << light.type << " is not a valid light type for Lighting.cpp's lightGeometry()" << std::endl;
        return g;
    }

    // directional
    if (light.type == 1) {
        g.direction = -glm::normalize(light.direction);
        g.distance = glm::length(light.direction);
        g.attenuation = 1.0f;

        return g;
    }

    // point light
    glm::vec3 const surfaceToLight = light.position - point;

    g.distance = glm::length(surfaceToLight);
    if (g.distance < 1e-6f) {
        g.distance = 0.01f;
        g.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    } 
    else {
        g.direction = -glm::normalize(surfaceToLight);
    }

    g.attenuation = 1 / (
        light.constant +
        light.linear * g.distance + 
        light.quadratic * g.distance*g.distance +
        1e-5f // avoids divide by 0
    );

    return g;
}

glm::vec3 phongContribution(const SceneLightSample& light,
                            const Material& mat,
                            const glm::vec3& baseColor,
                            const glm::vec3& normal,
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

    glm::vec3 const ambient = mat.ambient * light.ambient;

    glm::vec3 const diffuse = mat.diffuse * light.diffuse * glm::max(0.0f, glm::dot(normal, lightDir)) * reach;
 
    glm::vec3 const reflection = lightDir - 2 * glm::dot(normal, lightDir) * normal;
    float const reflectionViewDot = glm::dot(reflection, viewDir);
    glm::vec3 const specular = mat.specular * light.specular * std::powf(reflectionViewDot, mat.shininess);

    return (baseColor * (ambient + diffuse)) + specular;
}
