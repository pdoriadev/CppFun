#version 330 core

#define MAX_LIGHTS 8

#define LIGHT_POINT       0
#define LIGHT_DIRECTIONAL 1

struct Light {
    int   type;
    vec3  position;    // point lights
    vec3  direction;   // directional lights: the direction light travels
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float constant;    // attenuation: 1 / (constant + linear*d + quadratic*d*d)
    float linear;
    float quadratic;
};

struct Material {
    vec3  color;
    float ambient;     // ka
    float diffuse;     // kd
    float specular;    // ks
    float shininess;   // Phong exponent (was hardcoded to 32.0)
};

uniform Light lights[MAX_LIGHTS];
uniform int   numLights;

uniform Material material;
uniform vec3 viewPos;
uniform int  useLighting;

// Albedo texture. hasTexture is false unless the shape has BOTH a texture path
// and UVs, which is what keeps untextured rendering bit-for-bit identical to
// what it was before texturing existed -- the branch below is simply not taken.
uniform sampler2D albedoMap;
uniform bool      hasTexture;

in vec3 FragPos;
in vec3 Normal;
in vec3 FragColor;
in vec2 TexCoord;

out vec4 FragColorOutput;

void main() {
    if (useLighting == 0) {
        FragColorOutput = vec4(FragColor, 1.0);   // per-vertex colour, unlit
        return;
    }

    // Modulate rather than replace: the texture tints the material colour, so
    // a white-ish albedo map on a coloured shape still shows the colour, and
    // material.color alone is used when there is no map. The ray tracer does
    // exactly the same multiply in shade(), which is what keeps a trace and
    // the viewport agreeing.
    vec3 albedo = material.color;
    if (hasTexture) {
        albedo *= texture(albedoMap, TexCoord).rgb;
    }

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Two-sided shading.
    //
    // A one-sided surface (a Mobius strip) or a mesh with inconsistent winding
    // has normals pointing away from the viewer on some fragments. Without this
    // flip, dot(norm, lightDir) goes negative there, diffuse and specular clamp
    // to zero, and those areas render at ambient only - they look black.
    //
    // The ray tracer already does exactly this (see BVH.cpp), so orienting here
    // too is what keeps the viewport and a render of the same scene agreeing.
    if (dot(norm, viewDir) < 0.0) norm = -norm;

    vec3 result = vec3(0.0);

    // Accumulate every active light. numLights == 0 leaves the surface black,
    // so the application always keeps at least one light in the scene.
    for (int i = 0; i < numLights && i < MAX_LIGHTS; ++i) {

        vec3  lightDir;
        float attenuation = 1.0;

        if (lights[i].type == LIGHT_DIRECTIONAL) {
            // Parallel rays: direction is constant and there is no falloff.
            lightDir = normalize(-lights[i].direction);
        } else {
            vec3 toLight = lights[i].position - FragPos;
            float dist   = length(toLight);
            lightDir     = (dist > 0.0) ? toLight / dist : vec3(0.0, 1.0, 0.0);

            float denom = lights[i].constant
                        + lights[i].linear * dist
                        + lights[i].quadratic * dist * dist;
            attenuation = (denom > 0.0) ? 1.0 / denom : 1.0;
        }

        vec3 ambient = lights[i].ambient * material.ambient * albedo;

        float diff   = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lights[i].diffuse * material.diffuse * diff * albedo;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular   = lights[i].specular * material.specular * spec;

        result += (ambient + diffuse + specular) * attenuation;
    }

    FragColorOutput = vec4(result, 1.0);
}
