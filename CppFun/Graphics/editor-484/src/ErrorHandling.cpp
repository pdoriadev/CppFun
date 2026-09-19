#include "ErrorHandling.h"
#include <iostream>
#include <fstream>
#include <glad/glad.h>   // For OpenGL error checking
#include <GLFW/glfw3.h>  // For GLFW error callback

void ErrorHandling::glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
    logError("GLFW Error: " + std::string(description));
}

void ErrorHandling::checkOpenGLError(const std::string& context) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error in " << context << ": " << std::hex << err << std::endl;
        logError("OpenGL Error in " + context + ": " + std::to_string(err));
    }
}

void ErrorHandling::logError(const std::string& message) {
    std::ofstream logFile("error_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    }
}

void ErrorHandling::showError(const std::string& title, const std::string& message) {
    // Placeholder for platform-specific dialog (e.g., Windows MessageBox or ImGui)
    std::cerr << "[" << title << "] " << message << std::endl;
}

void ErrorHandling::enableOpenGLDebugging() {
    if (!GLAD_GL_VERSION_4_3 && !GLAD_GL_KHR_debug) {
        std::cerr << "OpenGL Debugging is not supported on this system." << std::endl;
        return;
    }

    glEnable(GL_DEBUG_OUTPUT); // Enable OpenGL debugging
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Make the output synchronous for easier debugging
    glDebugMessageCallback(openglDebugCallback, nullptr); // Set the debug callback function
}

void GLAPIENTRY ErrorHandling::openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                                   GLenum severity, GLsizei length,
                                                   const GLchar* message, const void* userParam) {
    std::cerr << "OpenGL Debug Message: " << message << std::endl;
    std::cerr << "Source: ";
    switch (source) {
        case GL_DEBUG_SOURCE_API: std::cerr << "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cerr << "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: std::cerr << "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: std::cerr << "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: std::cerr << "Other"; break;
    }
    std::cerr << "\nType: ";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: std::cerr << "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Deprecated Behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cerr << "Undefined Behavior"; break;
        case GL_DEBUG_TYPE_PORTABILITY: std::cerr << "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: std::cerr << "Performance"; break;
        case GL_DEBUG_TYPE_MARKER: std::cerr << "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: std::cerr << "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: std::cerr << "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: std::cerr << "Other"; break;
    }
    std::cerr << "\nSeverity: ";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: std::cerr << "High"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Medium"; break;
        case GL_DEBUG_SEVERITY_LOW: std::cerr << "Low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Notification"; break;
    }
    std::cerr << "\n-----------------------\n";
}