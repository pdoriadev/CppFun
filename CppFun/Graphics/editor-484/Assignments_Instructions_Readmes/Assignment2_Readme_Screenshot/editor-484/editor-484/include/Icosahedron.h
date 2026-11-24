#ifndef ICOSAHEDRON_H
#define ICOSAHEDRON_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Icosahedron  : public Shape {
public:
    const char* serialType() const override { return "Icosahedron"; }
    Icosahedron (float x, float y, float z, float uniformScale, int colorIndex, int id,
           float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, bool useUniformScaling = true);
    ~Icosahedron ();

    void draw(GLuint shaderProgram) override; // Render the icosahedron  shape

private:
    GLuint VAO, VBO, EBO; // OpenGL handles for icosahedron  geometry
    void setupIcosahedron ();   // Initializes the VAO/VBO/EBO for the icosahedron  shape
};

#endif

