#include "../include/Sphere.h"
#include "../include/TexCoords.h"

#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>

Sphere::Sphere(float x, float y, float z, float scale, int colorIndex, int id)
    : Shape(x, y, z, scale, colorIndex, id), VAO(0), VBO(0), EBO(0) {
    shapeType = "Sphere";  // Set the type as "Sphere"
    
    setupSphere(); // Initialize OpenGL objects for the sphere
}

Sphere::~Sphere() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Sphere::setupSphere() {
    std::cout << "SETUP SPHERE STARTED" << std::endl;
    texCoords.clear();

    // Resolution and size, GIVEN and deliberately outside the marked region
    // below: the texture-coordinate code further down needs the two segment
    // counts to build its grid, so they have to survive when the geometry is
    // stripped. They are yours to read and use.
    unsigned int const LATITUDE_SEGMENTS = 20; // Number of latitude lines
    unsigned int const LONGITUDE_SEGMENTS = 20; // Number of longitude lines
    float const radius = 0.5f;

    for (uint32_t i = 0; i < LATITUDE_SEGMENTS; ++i) {
        float const phi = static_cast<float>(i) / static_cast<float>(LATITUDE_SEGMENTS-1) * ShapeMath::PI();
        float const y = radius * glm::cos(phi);
        for (uint32_t j = 0; j < LONGITUDE_SEGMENTS; ++j) {
            float const theta = static_cast<float>(j) / static_cast<float>(LONGITUDE_SEGMENTS-1) * 2*ShapeMath::PI();
            float const x = radius * glm::sin(phi) * glm::cos(theta);
            float const z = radius * glm::sin(phi) * glm::sin(theta);

            vertices.push_back(glm::vec3(x, y, z));
            // normal = pos - origin. origin = vec3(0,0,0)
            normals.push_back(glm::normalize(glm::vec3(x, y, z))); 

            if (i == 0) continue;
            if (j == 0) continue; 
            
            // Intermediate plane
            int indices[4];
            indices[0] = (i - 1) * LATITUDE_SEGMENTS + j - 1; 
            indices[1] = i * LATITUDE_SEGMENTS + j - 1; 
            indices[2] = i * LATITUDE_SEGMENTS + j; 
            indices[3] = (i - 1) * LATITUDE_SEGMENTS + j; 
            
            faces.push_back({indices[0], indices[1], indices[2]});
            faces.push_back({indices[2], indices[3], indices[0]});
        }
    }

    unsigned int const TOTAL_VERTICES = LATITUDE_SEGMENTS * LONGITUDE_SEGMENTS;
    unsigned int const VALUES_PER_VERTEX = 9;
    if (vertices.size() != TOTAL_VERTICES) std::cerr << "INCORRECT VERTEX COUNT? Expected = " 
                                            << TOTAL_VERTICES << ". Actual = " << vertices.size() << std::endl;

    // Prepare OpenGL buffers using the populated attributes
    std::vector<float> vertexData;
    vertexData.reserve(TOTAL_VERTICES * 9); // 3N values = N pos values + N norm values + N col values
    std::vector<unsigned int> elementData;
    elementData.reserve(faces.size() * 3); // each face has 3 values. 

    // add vertices, normals, and color data to vertexData
    for (uint32_t i = 0; i < TOTAL_VERTICES; ++i) {
        // positions
        vertexData.push_back(vertices[i].x);
        vertexData.push_back(vertices[i].y);
        vertexData.push_back(vertices[i].z);

        // normals
        vertexData.push_back(normals[i].x);
        vertexData.push_back(normals[i].y);
        vertexData.push_back(normals[i].z);

        // color
        float const newColor = static_cast<float>(i) / static_cast<float>(TOTAL_VERTICES);
        vertexData.push_back(newColor);
        vertexData.push_back(newColor);
        vertexData.push_back(newColor);
    }

    outputVertices(vertexData, VALUES_PER_VERTEX);

    // add faces data to element index data
    for (uint32_t i = 0; i < faces.size(); ++i) {
        for (uint32_t j = 0; j < 3; ++j) {
            elementData.push_back(faces[i][j]);
        }
    }

    outputElements(elementData, 3);

    // COME BACK TO THIS LATER. I THINK IT'S READY????? CHECK THAT USING PROPER VALUES IN
        // VAO, VBO, EBO SETUP. AND IN DRAW FUNCTION.  

    // I DON'T KNOW HOW THE COLORINDEX SYSTEM WORKS ????
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

    // The natural parameterisation is one UV per UNIQUE vertex, which is what
    // TexCoords::sphere produces and what the ray tracer reads. The GL buffer
    // is not indexed that way: the loop below expands every face into three
    // fresh vertices and indexData is simply 0, 1, 2, ..., so the UV array
    // uploaded to attribute 3 has to be expanded to match.
    //
    // Getting this wrong is invisible in a ray-traced render and wrong in the
    // viewport: attribute 3 runs off the end of a too-short buffer after the
    // first ring and every vertex past it reads zero.
    // const std::vector<glm::vec2> vertexUVs =
    //     TexCoords::sphere(latitudeSegments, longitudeSegments);
    // texCoords.clear();
    // texCoords.reserve(faces.size() * 3);

    // for (const auto& face : faces) {
    //     for (int vertexIndex : face) {
    //         glm::vec3 const& position = vertices[vertexIndex];
    //         glm::vec3 const& normal = normals[vertexIndex];

    //         vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
    //         vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
    //         vertexData.insert(vertexData.end(), {color.r, color.g, color.b});

    //         if (static_cast<size_t>(vertexIndex) < vertexUVs.size()) {
    //             texCoords.push_back(vertexUVs[vertexIndex]);
    //         } else {
    //             texCoords.push_back(glm::vec2(0.0f));
    //         }
    //     }
    // }

    // Does this even make sense?? 
    // Vertex Elements are stored in vertexData.
    // Faces stores int arrays of size 3 for each face. 
    // Each value in a face array maps to a vertex element's index in vertexData. These three vertexElements make a triangle.
    // The index of a face array does not inherently map to anything. 
    // The *values* stored in each face array map to a vertex element index in vertexData like so:
    //      faces[i][j] * valuesPerVertex = vertexDataIndex. 
    // All this for loop does is store multiples of 3 with offsets.
    //  It's gotta be placeholder code. 
    // for (size_t i = 0; i < faces.size(); ++i) {
    //     elementData.insert(elementData.end(), {static_cast<unsigned int>(i * 3),
    //                                        static_cast<unsigned int>(i * 3 + 1),
    //                                        static_cast<unsigned int>(i * 3 + 2)});
    // }

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

    // UVs go in their own buffer on attribute 3, while the VAO is still bound.
    // uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO

    std::cout << "SETUP SPHERE FINISHED" << std::endl;
}

void Sphere::draw(GLuint shaderProgram) {

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

    // Disable lighting after drawing the sphere (for axis rendering)
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 0);
    }

}
