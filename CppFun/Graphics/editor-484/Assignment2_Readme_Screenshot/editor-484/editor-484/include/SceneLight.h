#ifndef SCENELIGHT_H
#define SCENELIGHT_H

#include <glm/glm.hpp>

// Single point light shared by the rasterizer and the ray tracer.
//
// This exists so the traced image and the OpenGL viewport agree. Before this,
// the light was hardcoded inside Renderer::setupLighting(); if you change it
// there now, the ray tracer would disagree and the two images would diverge.
struct SceneLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    SceneLight()
        : position(0.0f, 6.0f, 12.0f),
          ambient(0.15f, 0.15f, 0.15f),
          diffuse(1.0f, 1.0f, 1.0f),
          specular(0.8f, 0.8f, 0.8f) {}
};

extern SceneLight g_sceneLight;

// Matches the exponent hardcoded in shaders/fragment_shader.glsl.
extern const float kSpecularExponent;

#endif // SCENELIGHT_H
