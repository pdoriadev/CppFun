#include "Pyramid.h"
#include "TexCoords.h"

Pyramid::Pyramid(float x, float y, float z, float uniformScale, int colorIndex, int id,
                 float scaleX, float scaleY, float scaleZ, bool useUniformScaling)
    : Shape(x, y, z, uniformScale, colorIndex, id, scaleX, scaleY, scaleZ, useUniformScaling), VAO(0), VBO(0), EBO(0) {
    shapeType = "Pyramid";

    setupPyramid();     // Prepare OpenGL buffers
}

Pyramid::~Pyramid() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Pyramid::setupPyramid() {
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    // TODO(geometry): the pyramid: a square base and four triangular sides
    // Build the shape: fill `vertices`, `faces`, and `normals` (directly or
    // by calling calculateNormals()). See ASSIGNMENTS.md, A2, for the
    // conventions -- roughly one unit across, centred on the origin,
    // counter-clockwise winding seen from outside, every face index a valid
    // index into `vertices`.
    //
    // What is here is a placeholder: a single square in the XY plane. It is
    // deliberately not the shape you were asked for -- it is here so the
    // editor runs, the Insert menu does something visible, and you can see
    // your geometry replace it as you write it. Read src/Torus.cpp first;
    // it is the worked example of a procedural shape.
    vertices = { {-0.5f, -0.5f, 0.0f}, { 0.5f, -0.5f, 0.0f},
                 { 0.5f,  0.5f, 0.0f}, {-0.5f,  0.5f, 0.0f} };
    normals  = { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} };
    faces    = { {0, 1, 2}, {0, 2, 3} };

    // Build vertex data and index data for OpenGL
    
    for (size_t i = 0; i < faces.size(); ++i) {
    
        glm::vec3 normal = normals[i]; // Assign face normal
        glm::vec3 color = (colorIndex == 31) 
            ? glm::vec3(
                customColor[0], 
                customColor[1], 
                customColor[2]
            )
            : glm::vec3(
                colorPresets[colorIndex].color[0], 
                colorPresets[colorIndex].color[1], 
                colorPresets[colorIndex].color[2]
             );

        for (int j = 0; j < 3; ++j) {
            int vertexIndex = faces[i][j];
            const glm::vec3& position = vertices[vertexIndex];

            // Append position, normal, and color to vertexData
            vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
            vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
            vertexData.insert(vertexData.end(), {color.r, color.g, color.b});
        }

        indexData.insert(indexData.end(), {static_cast<unsigned int>(i * 3), static_cast<unsigned int>(i * 3 + 1), static_cast<unsigned int>(i * 3 + 2)});
    }


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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

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

