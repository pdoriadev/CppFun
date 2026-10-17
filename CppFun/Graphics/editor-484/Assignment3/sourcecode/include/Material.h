#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

// Per-object surface properties.
//
// The base colour is NOT stored here - it still comes from Shape's colorIndex /
// customColor, so every existing shape keeps the appearance it already had and
// the 32-preset picker keeps working unchanged.
//
// The first four fields have direct equivalents in the rasterizer and are
// uploaded to the fragment shader. The last three only mean anything to the ray
// tracer: a forward rasterizer has no way to gather reflected or refracted
// light, so the viewport simply will not preview them.
struct Material {
    // --- shared with the rasterizer ---
    float ambient;      // ka - scales the light's ambient term
    float diffuse;      // kd - scales the Lambert term
    float specular;     // ks - scales the Phong highlight
    float shininess;    // Phong exponent; was hardcoded to 32.0 in the shader

    // --- ray tracer only ---
    float reflectivity;       // 0 = matte, 1 = perfect mirror
    float transparency;       // 0 = opaque, 1 = fully transmissive
    float indexOfRefraction;  // glass ~1.5, water ~1.33

    // --- texturing ---
    // The PATH, not a GL handle. TextureCache resolves it, so the scene file
    // round-trips a string and a texture shared by six shapes is decoded once.
    // Empty means untextured, and untextured rendering must stay byte-identical
    // to what it was before textures existed -- the exact image sums in
    // raytrace_test are the tripwire for that.
    std::string texturePath;

    Material()
        : ambient(1.0f),
          diffuse(1.0f),
          specular(1.0f),
          shininess(32.0f),
          reflectivity(0.0f),
          transparency(0.0f),
          indexOfRefraction(1.5f) {}
};

#endif // MATERIAL_H
