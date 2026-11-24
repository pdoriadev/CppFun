#ifndef TEAPOT_H
#define TEAPOT_H

#include "Shape.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Teapot : public Shape {
public:
    const char* serialType() const override { return "Teapot"; }
    Teapot(float x, float y, float z, float uniformScale, int colorIndex, int id,
           float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, bool useUniformScaling = true);
    ~Teapot();

    void draw(GLuint shaderProgram) override; // Render the teapot

private:
    GLuint VAO, VBO, EBO; // OpenGL handles for teapot geometry
    void setupTeapot();   // Initializes the VAO/VBO/EBO for the teapot
};

#endif

