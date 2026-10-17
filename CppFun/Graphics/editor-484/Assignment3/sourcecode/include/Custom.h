#ifndef CUSTOM_H
#define CUSTOM_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Custom : public Shape {
public:
    const char* serialType() const override { return "Custom"; }
    Custom(float x, float y, float z, float uniformScale, int colorIndex, int id,
           float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, bool useUniformScaling = true);
    ~Custom();

    void draw(GLuint shaderProgram) override; // Render the custom shape

private:
    GLuint VAO, VBO, EBO; // OpenGL handles for custom geometry
    void setupCustom();   // Initializes the VAO/VBO/EBO for the custom shape
};

#endif

// how this should work
// shape construction should be somewhere.

// Shape constructor object. 
// shape constructor takes a parameter saying which shape to construct and return. 
// the shape constructor always returns vertexData array. Can return an elementData array.
// Encapsulated in a class object to allow for threading. 

// shape class should just have data. 
// methods/functions for constructing that data should be in separate classes. s