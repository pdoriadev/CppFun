#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sphere : public Shape {
public:
    const char* serialType() const override { return "Sphere"; }
    Sphere(float x, float y, float z, float scale, int colorIndex, int id);
    ~Sphere();

    void draw(GLuint shaderProgram) override;

private:
    GLuint VAO, VBO, EBO; // OpenGL handles for sphere geometry
    void setupSphere();   // Initializes the sphere's geometry
};

#endif
