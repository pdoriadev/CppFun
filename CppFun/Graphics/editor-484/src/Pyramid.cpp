#include "../include/Pyramid.h"
#include "../include/TexCoords.h"

#include <glm/geometric.hpp>
#include <iostream>

Pyramid::Pyramid(float x, float y, float z, float uniformScale, int colorIndex, int id,
                 float scaleX, float scaleY, float scaleZ, bool useUniformScaling)
    : Shape(x, y, z, uniformScale, colorIndex, id, scaleX, scaleY, scaleZ, useUniformScaling), VAO(0), VBO(0), EBO(0) {
    shapeType = "Pyramid";

    if (setupPyramid() == false) { std::cerr << "FAILED TO CONSTRUCT PYRAMID" << std::endl;};     // Prepare OpenGL buffers
}

Pyramid::~Pyramid() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

bool Pyramid::setupPyramid() {
    normalConfig = NormalConfig::EACH_VERTEX;

    glm::vec3 const pos[5] = {   
        glm::vec3( 0.0f,  0.77f,  0.0f),        // top center. 
        glm::vec3(-0.5f, -0.77f, -0.5f),      // bottom face. back left
        glm::vec3( 0.5f, -0.77f, -0.5f),      // bottom face. back right
        glm::vec3( 0.5f, -0.77f,  0.5f),      // bottom face. front right
        glm::vec3(-0.5f, -0.77f,  0.5f)       // bottom face. front left
    };

    glm::vec3 const color = glm::vec3(1.0f, 0.4f, 0.2f);

    std::cout << "Setup pos and normal data" << std::endl;

    // side tri's
    makeTri({pos[0], pos[2], pos[1]});
    makeTri({pos[0], pos[3], pos[2]});
    makeTri({pos[0], pos[4], pos[3]});
    makeTri({pos[0], pos[1], pos[4]});
    // bottom tri's
    makeTri({pos[1], pos[2], pos[3]});
    makeTri({pos[3], pos[4], pos[1]});

    faces = {
        {0, 2, 1},
        {3, 5, 4},
        {6, 8, 7},
        {9, 11, 10},
        {12, 14, 13},
        {15, 17, 16}
    };
    
    std::cout << "Total Faces = " << faces.size() << std::endl;

    std::vector<float> vertexData;
    vertexData.reserve(6 * 9); // 6 triangles * 9 floats per triangle
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertexData.push_back(vertices.at(i)[0]);
        vertexData.push_back(vertices.at(i)[1]);
        vertexData.push_back(vertices.at(i)[2]);

        vertexData.push_back(normals.at(i)[0]);
        vertexData.push_back(normals.at(i)[1]);
        vertexData.push_back(normals.at(i)[2]);

        vertexData.push_back(color.x);
        vertexData.push_back(color.y);
        vertexData.push_back(color.z);
    }

    outputVertices(vertexData, 9);

    std::vector<unsigned int> elementData;
    elementData.reserve(6 * 3); // 6 triangles * 3 verts per triangle. 
    for (size_t i = 0; i < faces.size(); ++i) {
        for (size_t j = 0; j < faces.at(i).size(); ++j) {
            elementData.push_back(faces.at(i).at(j));
            if (j >= 3) {
                std::cerr << "Too many verts for a triangle." << std::endl;
                return false;
            }
        }
    }

    std::cout << "setup element data" << std::endl;

    // Planar per-face projection. Returns one UV per triangle corner, which is
    // exactly what the emission loop above produced -- indexData is sequential,
    // so attribute 3 must be the same length as the interleaved buffer.
    texCoords = TexCoords::planarPerFace(vertices, faces);

    // Create and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); 

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementData.size() * sizeof(unsigned int), elementData.data(), GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // Color
    glEnableVertexAttribArray(2);

    // UVs in their own buffer on attribute 3, while the VAO is still bound.
    uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO

    std::cout << "Finished pyramid setup" << std::endl;
    return true;
}

void Pyramid::draw(GLuint shaderProgram) {

    // Use the shader program
    glUseProgram(shaderProgram);
    
    // Enable lighting for the cube
    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 1); // Enable lighting for the cube
    }

    // Apply transformations and pass to the shader
    applyTransform(shaderProgram);

    // Pass material properties
    GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
    if (colorLoc != -1) {
        glUniform3fv(colorLoc, 1, (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
    }

    // Render the cube
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faces.size() * 3), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Disable lighting after drawing the cube (for axis rendering)
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 0);
    }
}

