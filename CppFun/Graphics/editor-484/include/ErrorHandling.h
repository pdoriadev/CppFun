#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <iostream>
#include <string>
#include <glad/glad.h>
// glm 1.0+ hard-errors on gtx/ headers unless experimental extensions are
// explicitly enabled. Older glm (0.9.9, what Ubuntu ships) tolerated it, so
// this only shows up on newer toolchains such as MSYS2's glm 1.0.3.
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/string_cast.hpp>

class ErrorHandling {
public:
    // Handle GLFW errors
    static void glfwErrorCallback(int error, const char* description);

    // Handle OpenGL errors
    static void checkOpenGLError(const std::string& context);

    // Log errors to a file
    static void logError(const std::string& message);

    // Show error message (e.g., in a dialog)
    static void showError(const std::string& title, const std::string& message);

    // Initialize OpenGL Debugging
    static void enableOpenGLDebugging();

    // OpenGL Debug Callback Function
    static void GLAPIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                               GLenum severity, GLsizei length,
                                               const GLchar* message, const void* userParam);

};

#endif // ERRORHANDLING_H
