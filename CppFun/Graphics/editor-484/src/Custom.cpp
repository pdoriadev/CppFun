#include "../include/Custom.h"

#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>

#include <cmath>

namespace {
// sign(w) * |w|^e. Raising a negative base to a fractional power is NaN, so
// the sign is stripped and reapplied.
float signedPow(float base, float exponent) {
    float magnitude = std::pow(std::fabs(base), exponent);
    return (base < 0.0f) ? -magnitude : magnitude;
}
} // namespace


Custom::Custom(float x, float y, float z, float uniformScale, int colorIndex, int id,
           float scaleX, float scaleY, float scaleZ, bool useUniformScaling) 
           : Shape(x, y, z, uniformScale, colorIndex, id), VAO(0), VBO(0), EBO(0) {
    shapeType = customShapeName;  // Set the type as "Sphere"
    
    setupCustom(); // Initialize OpenGL objects for the sphere
}

Custom::~Custom() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Custom::setupCustom() {
    std::cout << "SETUP T-SPHERE STARTED" << std::endl;
    texCoords.clear();
    
    // Seed with a real random value, if available
    std::random_device randomDevice;
    
    // Choose a random mean between 0 and 1
    std::default_random_engine e1(randomDevice());
    std::uniform_real_distribution<float> uniform_dist(0, 1); // min max values
    
    unsigned int const LATITUDE_SEGMENTS = 200; // Number of latitude lines
    unsigned int const LONGITUDE_SEGMENTS = 200; // Number of longitude lines
    float const RADIUS = 0.5f;

    float const MAX_TESSELLATION_DISTANCE = 2 * static_cast<float>(1) / LATITUDE_SEGMENTS;

    for (uint32_t i = 0; i < LATITUDE_SEGMENTS; ++i) {
        float const PHI = static_cast<float>(i) / static_cast<float>(LATITUDE_SEGMENTS-1) * ShapeMath::PI();
        float const PRE_Y = RADIUS * glm::cos(PHI);
        
        for (uint32_t j = 0; j < LONGITUDE_SEGMENTS; ++j) {
            float const THETA = static_cast<float>(j) / static_cast<float>(LONGITUDE_SEGMENTS-1) * 2*ShapeMath::PI();
            float const X = RADIUS * glm::sin(PHI) * glm::cos(THETA);
            float const Z = RADIUS * glm::sin(PHI) * glm::sin(THETA);

            float const SIGN = PRE_Y < 0 ? -1.0f : 1.0f;
            //float const Y = SIGN * PRE_Y * PRE_Y;
            float const Y = SIGN * PRE_Y * PRE_Y * PRE_Y * PRE_Y 
                            + MAX_TESSELLATION_DISTANCE * 2 * glm::cos(X * 6 * ShapeMath::PI()) ;

            glm::vec3 pos(X, Y, Z);
            // tessellate
            pos += glm::normalize(pos) * MAX_TESSELLATION_DISTANCE * uniform_dist(e1);

            vertices.push_back(pos);
            // normal = pos - origin. origin = vec3(0,0,0)
            // assign dummy value to normal 
            normals.push_back(glm::vec3(0, 0, 0)); 

            if (i == 0) continue;
            if (j == 0) continue; 
            
            // Construct Plane
            int indices[4];
            indices[0] = (i - 1) * LATITUDE_SEGMENTS + j - 1; 
            indices[1] = i * LATITUDE_SEGMENTS + j - 1; 
            indices[2] = i * LATITUDE_SEGMENTS + j; 
            indices[3] = (i - 1) * LATITUDE_SEGMENTS + j; 
            faces.push_back({indices[0], indices[1], indices[2]});
            faces.push_back({indices[2], indices[3], indices[0]});

            {
                glm::vec3 const BOT_LEFT_TO_TOP_LEFT = vertices[indices[1]] - vertices[indices[0]]; 
                glm::vec3 const BOT_LEFT_TO_BOT_RIGHT = vertices[indices[2]] - vertices[indices[0]]; 
                glm::vec3 const PLANE_NORMAL = glm::normalize(glm::cross(BOT_LEFT_TO_BOT_RIGHT, BOT_LEFT_TO_TOP_LEFT));
                for (uint32_t i = 0; i < 4; ++i ) {
                    normals[indices[i]] = PLANE_NORMAL;
                }
            }

            if (j != LONGITUDE_SEGMENTS - 1) continue;

            // Construct 'glue' wrap-around plane
            indices[0] = (i - 1) * LATITUDE_SEGMENTS + j;
            indices[1] = i * LATITUDE_SEGMENTS + j;
            indices[2] = i * LATITUDE_SEGMENTS;
            indices[3] = (i - 1) * LATITUDE_SEGMENTS;
            faces.push_back({indices[0], indices[1], indices[2]});
            faces.push_back({indices[2], indices[3], indices[0]});

            {
                glm::vec3 const BOT_LEFT_TO_TOP_LEFT = vertices[indices[1]] - vertices[indices[0]]; 
                glm::vec3 const BOT_LEFT_TO_BOT_RIGHT = vertices[indices[2]] - vertices[indices[0]]; 
                glm::vec3 const PLANE_NORMAL = glm::normalize(glm::cross(BOT_LEFT_TO_BOT_RIGHT, BOT_LEFT_TO_TOP_LEFT));
                for (uint32_t i = 0; i < 4; ++i ) {
                    normals[indices[i]] = PLANE_NORMAL;
                }
            }
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

void Custom::draw(GLuint shaderProgram) {

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
