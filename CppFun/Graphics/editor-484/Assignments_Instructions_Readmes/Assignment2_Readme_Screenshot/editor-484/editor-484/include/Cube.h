#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class Cube : public Shape {
public:
    const char* serialType() const override { return "Cube"; }
    Cube(float x, float y, float z, float scale, int colorIndex, int id);
    ~Cube();

    void draw(GLuint shaderProgram) override; // Render the cube
    
    private:
    GLuint VAO, VBO, EBO; // OpenGL handles for the cube geometry
    bool setupCube();     // Initializes the VAO/VBO/EBO for the cube
};

#endif
