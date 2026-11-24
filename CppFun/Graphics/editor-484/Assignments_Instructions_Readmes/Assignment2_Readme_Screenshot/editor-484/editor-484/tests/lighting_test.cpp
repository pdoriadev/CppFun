// Grader for the `lighting` topic: Phong local illumination.
//
// GL-free and scene-free -- two pure functions and a handful of vectors, so it
// runs in milliseconds and can be run on every edit.
//
// The assertions are written as PROPERTIES rather than expected numbers
// wherever a property will do: a surface facing the light is brighter than one
// at an angle, a highlight is strongest along the mirror direction, doubling
// the distance dims a point light. Those hold for any correct implementation
// and stay true if the constants are ever retuned. Hardcoded values appear only
// where the value is the contract -- the ambient term, and the agreement with
// the GLSL.

#include "Lighting.h"

#include <cstdio>
#include <cmath>
#include <string>

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

static bool close(float a, float b, float eps = 1e-4f) {
    return std::fabs(a - b) < eps;
}

// A white point light at the origin, no falloff, so a test that is not about
// attenuation is not accidentally about attenuation.
static SceneLightSample whiteLight(const glm::vec3& pos) {
    SceneLightSample L;
    L.type      = 0;
    L.position  = pos;
    L.ambient   = glm::vec3(0.1f);
    L.diffuse   = glm::vec3(1.0f);
    L.specular  = glm::vec3(1.0f);
    L.constant  = 1.0f;
    L.linear    = 0.0f;
    L.quadratic = 0.0f;
    return L;
}

static Material plainMaterial() {
    Material m;
    m.ambient   = 0.2f;
    m.diffuse   = 0.8f;
    m.specular  = 0.5f;
    m.shininess = 32.0f;
    return m;
}

int main() {
    std::printf("Phong local illumination\n");

    const glm::vec3 white(1.0f);
    const glm::vec3 up(0.0f, 1.0f, 0.0f);

    // --- light geometry -----------------------------------------------------
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 5.0f, 0.0f));
        const LightGeometry g = lightGeometry(L, glm::vec3(0.0f));

        ok(close(g.direction.y, 1.0f) && close(glm::length(g.direction), 1.0f),
           "a point light overhead is straight up, and the direction is a UNIT "
           "vector -- an unnormalised one silently scales every later dot product");
        ok(close(g.distance, 5.0f), "at the right distance");
        ok(close(g.attenuation, 1.0f),
           "constant 1 with no linear or quadratic term means no falloff");
    }

    // Attenuation. Not a fixed number -- the property that matters is that a
    // point light dims with distance and a directional one does not.
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 1.0f, 0.0f));
        L.quadratic = 1.0f;

        const float near = lightGeometry(L, glm::vec3(0.0f)).attenuation;
        L.position = glm::vec3(0.0f, 2.0f, 0.0f);
        const float far = lightGeometry(L, glm::vec3(0.0f)).attenuation;

        ok(far < near, "a point light dims as the surface moves away");
        ok(close(near, 0.5f) && close(far, 0.2f),
           "and by 1/(constant + linear*d + quadratic*d*d), which is what the "
           "Light panel exposes and what a scene file means");
    }

    // Directional: parallel rays, so no position and no falloff.
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 100.0f, 0.0f));
        L.type      = 1;
        L.direction = glm::vec3(0.0f, -1.0f, 0.0f);   // travelling downwards
        L.quadratic = 1.0f;                           // must be ignored

        const LightGeometry a = lightGeometry(L, glm::vec3(0.0f));
        const LightGeometry b = lightGeometry(L, glm::vec3(0.0f, -50.0f, 0.0f));

        ok(close(a.direction.y, 1.0f),
           "a directional light travelling DOWN is seen looking UP -- the stored "
           "direction is the way the light goes, not the way to it");
        ok(close(a.attenuation, 1.0f) && close(b.attenuation, 1.0f),
           "and it does not fall off, however far away the surface is");
        ok(close(a.direction.x, b.direction.x) && close(a.direction.z, b.direction.z),
           "its direction is the same everywhere: the rays are parallel");
    }

    // --- the reflectance model ----------------------------------------------
    const Material mat = plainMaterial();

    // Ambient is the floor: present with the light fully blocked, present with
    // the surface facing away.
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 5.0f, 0.0f));
        const glm::vec3 shadowed =
            phongContribution(L, mat, white, up, up, up, 0.0f);

        ok(close(shadowed.r, 0.1f * 0.2f),
           "a fully shadowed surface still gets light.ambient * mat.ambient * "
           "colour -- ambient stands in for light that arrived by a path nobody "
           "traced, so a shadow ray says nothing about it");
        ok(shadowed.r > 0.0f,
           "which is why a shadow is dark and not black");
    }

    // Diffuse: Lambert.
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 5.0f, 0.0f));

        const glm::vec3 head = phongContribution(L, mat, white, up, up, up, 1.0f);

        // Same light, surface tilted 60 degrees away from it.
        const glm::vec3 tilted =
            phongContribution(L, mat, white,
                              glm::normalize(glm::vec3(0.866f, 0.5f, 0.0f)),
                              up, up, 1.0f);

        ok(head.r > tilted.r,
           "a surface facing the light is brighter than one at an angle");
        ok(tilted.r > 0.0f, "but still lit while any of it faces the light");

        // Facing away: the cosine goes negative and must clamp. Without the
        // clamp this light would SUBTRACT from whatever the others contributed.
        //
        // Specular is zeroed for this check on purpose. Phong's specular term is
        // NOT gated by N.L -- a surface facing away can still land in the mirror
        // lobe and light up, which is a real quirk of the model and one the GLSL
        // shares. Leaving it in would make this assertion about that quirk
        // instead of about the diffuse clamp.
        Material matte = mat;
        matte.specular = 0.0f;

        const glm::vec3 away =
            phongContribution(L, matte, white, glm::vec3(0.0f, -1.0f, 0.0f),
                              up, up, 1.0f);
        ok(close(away.r, 0.1f * 0.2f),
           "a surface facing away gets ambient only -- the cosine must clamp at "
           "zero, or this light subtracts from the others");

        // And the quirk itself, pinned so nobody "fixes" it into disagreeing
        // with the shader. Gating specular on N.L would be defensible in a
        // vacuum; it is not defensible when the rasterizer does not do it,
        // because then the viewport and a render stop matching.
        const glm::vec3 backlit =
            phongContribution(L, mat, white, glm::vec3(0.0f, -1.0f, 0.0f),
                              up, up, 1.0f);
        ok(backlit.r > away.r,
           "Phong's specular is not gated by N.L, matching the GLSL -- a "
           "surface facing away can still catch the mirror lobe");
    }

    // Specular: Phong, and the highlight follows the mirror direction.
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 5.0f, 0.0f));
        Material shiny = mat;
        shiny.diffuse = 0.0f;          // isolate the specular term
        shiny.ambient = 0.0f;

        // Light straight up, normal straight up: the mirror direction is
        // straight up too, so an eye directly above sees the whole highlight.
        const glm::vec3 onAxis =
            phongContribution(L, shiny, white, up, up, up, 1.0f);

        const glm::vec3 offAxis =
            phongContribution(L, shiny, white, up,
                              glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)), up, 1.0f);

        ok(onAxis.r > offAxis.r,
           "the highlight is strongest along the mirror direction and falls off "
           "away from it");
        ok(close(onAxis.r, 0.5f),
           "and at its centre it is light.specular * mat.specular");

        // Shininess is an exponent, so a higher one makes a TIGHTER highlight,
        // not a dimmer one: unchanged at the centre, darker off to the side.
        Material tight = shiny;
        tight.shininess = 128.0f;
        const glm::vec3 tightOn  = phongContribution(L, tight, white, up, up, up, 1.0f);
        const glm::vec3 tightOff =
            phongContribution(L, tight, white, up,
                              glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)), up, 1.0f);

        ok(close(tightOn.r, onAxis.r),
           "raising shininess does not dim the centre of the highlight");
        ok(tightOff.r < offAxis.r, "it narrows it");
    }

    // The highlight takes the LIGHT's colour, not the surface's. This is why a
    // red plastic ball has a white highlight, and it is the one place baseColor
    // must not appear.
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 5.0f, 0.0f));
        Material shiny = mat;
        shiny.diffuse = 0.0f;
        shiny.ambient = 0.0f;

        const glm::vec3 red(1.0f, 0.0f, 0.0f);
        const glm::vec3 c = phongContribution(L, shiny, red, up, up, up, 1.0f);

        ok(close(c.g, c.r) && close(c.b, c.r),
           "a highlight on a RED surface under a white light is white -- the "
           "specular term must not be multiplied by the surface colour");
    }

    // --- reach --------------------------------------------------------------
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 5.0f, 0.0f));

        const glm::vec3 clear = phongContribution(L, mat, white, up, up, up, 1.0f);
        const glm::vec3 half  = phongContribution(L, mat, white, up, up, up, 0.5f);
        const glm::vec3 none  = phongContribution(L, mat, white, up, up, up, 0.0f);

        ok(half.r < clear.r && half.r > none.r,
           "partial reach lands between clear and blocked -- this is what makes "
           "a shadow through glass different from a shadow through a wall");

        const float ambient = 0.1f * 0.2f;
        ok(close(half.r - ambient, (clear.r - ambient) * 0.5f),
           "and it scales the direct terms linearly, leaving ambient alone");
    }

    // --- agreement with the GLSL --------------------------------------------
    //
    // The contract that keeps the viewport and a ray-traced render of the same
    // scene looking alike. The expected value is worked out here by hand, in the
    // same order shaders/fragment_shader.glsl does it, so a change to either
    // side that is not made to the other shows up as a failure rather than as a
    // render that quietly disagrees with the screen.
    {
        SceneLightSample L = whiteLight(glm::vec3(0.0f, 3.0f, 0.0f));
        L.ambient   = glm::vec3(0.2f);
        L.diffuse   = glm::vec3(0.7f);
        L.specular  = glm::vec3(0.9f);
        L.linear    = 0.1f;

        const glm::vec3 point(0.0f);
        const glm::vec3 n = glm::normalize(glm::vec3(0.2f, 1.0f, 0.1f));
        const glm::vec3 viewDir = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));

        const LightGeometry g = lightGeometry(L, point);
        const glm::vec3 got =
            phongContribution(L, mat, white, n, viewDir, g.direction, 1.0f) * g.attenuation;

        // vec3 ambient  = light.ambient  * material.ambient  * albedo;
        // float diff    = max(dot(norm, lightDir), 0.0);
        // vec3 diffuse  = light.diffuse  * material.diffuse  * diff * albedo;
        // vec3 reflect  = reflect(-lightDir, norm);
        // float spec    = pow(max(dot(viewDir, reflect), 0.0), material.shininess);
        // vec3 specular = light.specular * material.specular * spec;
        // result += (ambient + diffuse + specular) * attenuation;
        const float diff = glm::max(glm::dot(n, g.direction), 0.0f);
        const glm::vec3 rd = glm::reflect(-g.direction, n);
        const float spec = std::pow(glm::max(glm::dot(viewDir, rd), 0.0f), mat.shininess);
        const float att = 1.0f / (1.0f + 0.1f * 3.0f);
        const float want = (0.2f * 0.2f + 0.7f * 0.8f * diff + 0.9f * 0.5f * spec) * att;

        ok(close(got.r, want, 1e-4f),
           "the CPU model matches the fragment shader term for term -- the "
           "viewport and a render of the same scene have to agree");
    }

    std::printf("\n%d checks, %d failures\n", checks, failures);
    if (failures == 0) { std::printf("ALL TESTS PASSED (0 failures)\n"); return 0; }
    std::printf("TESTS FAILED (%d failures)\n", failures);
    return 1;
}
