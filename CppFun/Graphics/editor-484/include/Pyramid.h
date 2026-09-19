#ifndef PYRAMID_H
#define PYRAMID_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Pyramid : public Shape {
public:
    const char* serialType() const override { return "Pyramid"; }
    Pyramid(float x, float y, float z, float uniformScale, int colorIndex, int id,
            float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, bool useUniformScaling = true);
    ~Pyramid();
    
    void draw(GLuint shaderProgram) override; // Render the pyramid
    
private:
    GLuint VAO, VBO, EBO; // OpenGL handles for pyramid geometry
    void setupPyramid();  // Initializes the VAO/VBO/EBO for the pyramid    
};

#endif
