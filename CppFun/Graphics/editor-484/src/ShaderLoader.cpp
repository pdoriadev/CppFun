#include "ShaderLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

GLuint ShaderLoader::loadShaderFromFile(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode, fragmentCode;
    try {
        std::ifstream vShaderFile(vertexPath);
        std::ifstream fShaderFile(fragmentPath);

        // CHECK THAT THE FILES OPENED, and say so in those words.
        //
        // These paths are relative to the WORKING DIRECTORY, not to the
        // executable, so running the editor from anywhere but its own folder
        // finds nothing. Without this check an unopened ifstream yields an
        // empty string, the empty string is handed to glCompileShader, and the
        // driver reports
        //
        //     0:1(1): error: syntax error, unexpected end of file
        //     ERROR: Vertex Shader Compilation Failed
        //
        // which sends the reader hunting through GLSL that is completely fine.
        // This matters most for the prebuilt solution binary: a student who
        // downloads it, drops the executable somewhere on its own and runs it
        // hits exactly that, and the message must name the real cause.
        if (!vShaderFile || !fShaderFile) {
            std::cerr << "ERROR: could not open shader source.\n";
            if (!vShaderFile) std::cerr << "  missing: " << vertexPath   << "\n";
            if (!fShaderFile) std::cerr << "  missing: " << fragmentPath << "\n";
            std::cerr << "  These paths are relative to the current directory, so\n"
                         "  run the editor from the folder that contains shaders/.\n";
            return 0;
        }

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR: Shader file not successfully read" << std::endl;
        return 0;
    }

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "ERROR: a shader source file is empty ("
                  << (vertexCode.empty() ? vertexPath : fragmentPath) << ")\n";
        return 0;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR: Vertex Shader Compilation Failed\n" << infoLog << std::endl;
    }

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR: Fragment Shader Compilation Failed\n" << infoLog << std::endl;
    }

    // Link shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader Program Linking Failed\n" << infoLog << std::endl;
    }

    // Cleanup
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderProgram;
}
