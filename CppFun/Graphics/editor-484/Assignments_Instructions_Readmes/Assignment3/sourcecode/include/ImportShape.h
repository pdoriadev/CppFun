#ifndef IMPORTSHAPE_H
#define IMPORTSHAPE_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

class ImportShape : public Shape {
public:
    const char* serialType() const override { return "ImportShape"; }
    ImportShape(float x, float y, float z, float scale, int colorIndex, int id);
    ~ImportShape();

    void draw(GLuint shaderProgram) override;
    void setupShape();

    // This class stores faces as interleaved vertex/normal index pairs
    // ({v0,n0, v1,n1, v2,n2}), unlike every other shape.
    int getFaceIndexStride() const override { return 2; }
    
 private:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;   
};

#endif
