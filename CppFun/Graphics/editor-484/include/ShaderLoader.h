#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include "glad/glad.h"

class ShaderLoader {
public:
    // Load and compile shaders from files
    static GLuint loadShaderFromFile(const char* vertexPath, const char* fragmentPath);
};

#endif // SHADERLOADER_H
