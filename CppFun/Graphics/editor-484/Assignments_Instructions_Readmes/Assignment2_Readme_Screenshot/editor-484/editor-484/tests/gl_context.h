// Minimal hidden-window GL context so headless tests can construct shapes,
// whose constructors call glGenVertexArrays etc.
#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
inline GLFWwindow* makeHeadlessContext() {
    if (!glfwInit()) { std::printf("glfwInit failed\n"); return 0; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* w = glfwCreateWindow(64, 64, "headless", 0, 0);
    if (!w) { std::printf("window creation failed\n"); return 0; }
    glfwMakeContextCurrent(w);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::printf("glad failed\n"); return 0;
    }
    return w;
}
