#include "ImportShape.h"

ImportShape::ImportShape(float x, float y, float z, float scale, int colorIndex, int id)
	: Shape(x, y, z, scale, colorIndex, id), VAO(0), VBO(0), EBO(0) {
	
//    setupShape();  // Prepare OpenGL buffers
	
}

ImportShape::~ImportShape() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void ImportShape::setupShape() {
    vertexData.clear();
    indexData.clear();

    const glm::vec3 color = getBaseColor();

    // Populate vertex and normal data
    for (size_t i = 0; i < faces.size(); ++i) {
        for (int j = 0; j < 3; ++j) { // Each face has 3 vertices
            int vertexIndex = faces[i][j * 2];  // Access vertex index
            int normalIndex = faces[i][j * 2 + 1]; // Access normal index
            
            const glm::vec3& position = vertices[vertexIndex];
            const glm::vec3& normal = normals[normalIndex];

            // Append position, normal and colour -- stride 9, the same layout
            // every other shape uses. This file used to write stride 8 with a
            // 2-float texture-coordinate placeholder in the third slot, which
            // meant attribute 2 was a vec2 here and a vec3 (aColor) everywhere
            // else, including in the vertex shader that reads it. It was
            // harmless only because FragColor is read solely when
            // useLighting == 0, and imported shapes always draw lit.
            vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
            vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
            vertexData.insert(vertexData.end(), {color.r, color.g, color.b});
        }

        // Add indices for this triangle
        indexData.insert(indexData.end(), {static_cast<unsigned int>(i * 3), 
                                           static_cast<unsigned int>(i * 3 + 1), 
                                           static_cast<unsigned int>(i * 3 + 2)});
    }

    // Generate OpenGL buffers
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

    // UVs, if the OBJ carried vt lines. They arrive one per triangle corner,
    // in the same order this loop emitted vertices, so no reindexing is needed.
    uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO
}

void ImportShape::draw(GLuint shaderProgram) {

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
