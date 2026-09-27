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
    uint32_t const VERTICES_PER_LOOP = 4;
    uint32_t const VALUES_PER_VERTEX = 9;
    uint32_t const MAX_LOOPS = 1;
    uint32_t const FACES_COUNT = (MAX_LOOPS * VERTICES_PER_LOOP) + 1;

    glm::vec3 const positions[ VERTICES_PER_LOOP + 1 ] = {   
        glm::vec3(0, 0.5, 0),               // top vertex
        glm::vec3(-0.5f, -0.5f, -0.5f),     // bottom face. bottom left
        glm::vec3( 0.5f, -0.5f, -0.5f),      // bottom face. bottom right
        glm::vec3( 0.5f, -0.5f,  0.5f),       // bottom face. top right
        glm::vec3(-0.5f, -0.5f,  0.5f)       // bottom face. top left
    };

    glm::vec3 normals[ FACES_COUNT ] = { 
        glm::vec3(0.0f), 
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f) 
    };

    std::vector<unsigned int> elementData;
    elementData.reserve(18); // 1 plane = 2 tris. (2 tris + 4 tris) * 3 vertices per tri

    // construct 3 intermediate side-tri's
    for (int32_t i = 1; i < FACES_COUNT - 1; ++i) {
        // side triangle indices
        faces.push_back({0, i, i+1});
        for (uint32_t i = 0; i < 3; ++i) {
            elementData.push_back(faces[faces.size() - 1][i]);
        }
        
        glm::vec3 const TOP_TO_BOTTOM_LEFT  = (positions[i] - positions[0] );
        glm::vec3 const TOP_TO_BOTTOM_RIGHT = (positions[i+1] - positions[0] ); // bottom to side is not necessarily perpendicular to the bottom to top. Need alternative algorithm
        glm::vec3 const PLANE_NORMAL        = glm::cross(TOP_TO_BOTTOM_LEFT, TOP_TO_BOTTOM_RIGHT);
        
        normals[0]      += PLANE_NORMAL;
        normals[i]      += PLANE_NORMAL;
        normals[i+1]    += PLANE_NORMAL;
    } 

    { // WRAP-AROUND TRI
        // side triangle indices
        faces.push_back({0, 4, 1});
        for (uint32_t i = 0; i < 3; ++i) {
            elementData.push_back(faces[faces.size() - 1][i]);
        }

        glm::vec3 const TOP_TO_BOTTOM_LEFT  = glm::normalize(positions[4] - positions[0]);
        glm::vec3 const TOP_TO_BOTTOM_RIGHT = glm::normalize(positions[1] - positions[0]);
        glm::vec3 const PLANE_NORMAL        = glm::cross(TOP_TO_BOTTOM_LEFT, TOP_TO_BOTTOM_RIGHT);

        normals[0] += PLANE_NORMAL;
        normals[4] += PLANE_NORMAL;
        normals[1] += PLANE_NORMAL;
    }

    { // BOTTOM-FACE
        // side triangle indices
        faces.push_back({4, 3, 2});
        for (uint32_t i = 0; i < 3; ++i) {
            elementData.push_back(faces[faces.size() - 1][i]);
        }

        faces.push_back({2, 1, 4});
        for (uint32_t i = 0; i < 3; ++i) {
            elementData.push_back(faces[faces.size() - 1][i]);
        }

        glm::vec3 const THREE_TO_FOUR  = glm::normalize(positions[4] - positions[3]);
        glm::vec3 const THREE_TO_TWO   = glm::normalize(positions[3] - positions[2]);
        glm::vec3 const PLANE_NORMAL   = glm::cross(THREE_TO_TWO, THREE_TO_FOUR);

        for (uint32_t i = 1; i < FACES_COUNT; ++i) {
            normals[i] += PLANE_NORMAL;
        }
    }
    
    // normalize normals
    for (uint32_t i = 1; i < FACES_COUNT; ++i) {
        normals[i] = glm::normalize(normals[i]);
    }
    
    std::vector<float> vertexData;
    vertexData.reserve( VALUES_PER_VERTEX * (VERTICES_PER_LOOP + 1) ); // accounts for single-top vertex
    
    // Add position, normal, and color data to vertex data. 
    for (uint32_t i = 0; i < (VERTICES_PER_LOOP + 1); ++i) {
        vertexData.push_back(positions[i].x);
        vertexData.push_back(positions[i].y);
        vertexData.push_back(positions[i].z);

        vertexData.push_back(normals[i].x);
        vertexData.push_back(normals[i].y);
        vertexData.push_back(normals[i].z);
        

        for (uint32_t j = 0; j < 3; ++j) {
            vertexData.push_back(static_cast<float>(i) / static_cast<float>(VERTICES_PER_LOOP + 1));
        }
        
        outputVertices(vertexData, VALUES_PER_VERTEX);
    }

    std::cout << "Total Faces = " << faces.size() << std::endl;

    //outputElements(elementData, 3);

    // Planar per-face projection. Returns one UV per triangle corner, which is
    // exactly what the emission loop above produced -- indexData is sequential,
    // so attribute 3 must be the same length as the interleaved buffer.
    //texCoords = TexCoords::planarPerFace(vertices, faces);

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
    // uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO

    //-////////////////////////////////////////////////////////
    // ORIGINAL CODE + COMMENTS

     // // TODO(geometry): the pyramid: a square base and four triangular sides
    // // Build the shape: fill `vertices`, `faces`, and `normals` (directly or
    // // by calling calculateNormals()). See ASSIGNMENTS.md, A2, for the
    // // conventions -- roughly one unit across, centred on the origin,
    // // counter-clockwise winding seen from outside, every face index a valid
    // // index into `vertices`.
    // //
    // // What is here is a placeholder: a single square in the XY plane. It is
    // // deliberately not the shape you were asked for -- it is here so the
    // // editor runs, the Insert menu does something visible, and you can see
    // // your geometry replace it as you write it. Read src/Torus.cpp first;
    // // it is the worked example of a procedural shape.
    // // vertices = { {-0.5f, -0.5f, 0.0f}, { 0.5f, -0.5f, 0.0f},
    // //              { 0.5f,  0.5f, 0.0f}, {-0.5f,  0.5f, 0.0f} };
    // // normals  = { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
    // //              {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} };
    // // faces    = { {0, 1, 2}, {0, 2, 3} };

    // // Build vertex data and index data for OpenGL
    
    // for (size_t i = 0; i < faces.size(); ++i) {
    
    //     glm::vec3 normal = normals[i]; // Assign face normal
    //     glm::vec3 color = (colorIndex == 31) 
    //         ? glm::vec3(
    //             customColor[0], 
    //             customColor[1], 
    //             customColor[2]
    //         )
    //         : glm::vec3(
    //             colorPresets[colorIndex].color[0], 
    //             colorPresets[colorIndex].color[1], 
    //             colorPresets[colorIndex].color[2]
    //          );

    //     for (int j = 0; j < 3; ++j) {
    //         int vertexIndex = faces[i][j];
    //         const glm::vec3& position = vertices[vertexIndex];

    //         // Append position, normal, and color to vertexData
    //         vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
    //         vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
    //         vertexData.insert(vertexData.end(), {color.r, color.g, color.b});
    //     }

    //     elementData.insert(elementData.end(), {static_cast<unsigned int>(i * 3), static_cast<unsigned int>(i * 3 + 1), static_cast<unsigned int>(i * 3 + 2)});
    // }

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

