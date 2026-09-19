#include "Sphere.h"
#include "TexCoords.h"

#include <vector>
#include <cmath>

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
    texCoords.clear();

    // Resolution and size, GIVEN and deliberately outside the marked region
    // below: the texture-coordinate code further down needs the two segment
    // counts to build its grid, so they have to survive when the geometry is
    // stripped. They are yours to read and use.
    const unsigned int latitudeSegments = 20; // Number of latitude lines
    const unsigned int longitudeSegments = 20; // Number of longitude lines
    const float radius = 0.5f;

    // TODO(geometry): the sphere: spherical coordinates over a latitude/longitude grid
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

    // Prepare OpenGL buffers using the populated attributes
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

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
    const std::vector<glm::vec2> vertexUVs =
        TexCoords::sphere(latitudeSegments, longitudeSegments);
    texCoords.clear();
    texCoords.reserve(faces.size() * 3);

    for (const auto& face : faces) {
        for (int vertexIndex : face) {
            const glm::vec3& position = vertices[vertexIndex];
            const glm::vec3& normal = normals[vertexIndex];

            vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
            vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
            vertexData.insert(vertexData.end(), {color.r, color.g, color.b});

            if (static_cast<size_t>(vertexIndex) < vertexUVs.size()) {
                texCoords.push_back(vertexUVs[vertexIndex]);
            } else {
                texCoords.push_back(glm::vec2(0.0f));
            }
        }
    }

    for (size_t i = 0; i < faces.size(); ++i) {
        indexData.insert(indexData.end(), {static_cast<unsigned int>(i * 3),
                                           static_cast<unsigned int>(i * 3 + 1),
                                           static_cast<unsigned int>(i * 3 + 2)});
    }

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

    // UVs go in their own buffer on attribute 3, while the VAO is still bound.
    uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO
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
