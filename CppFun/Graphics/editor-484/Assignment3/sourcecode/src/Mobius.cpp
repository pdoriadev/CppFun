#include "Mobius.h"
#include "TexCoords.h"

#include <cmath>
#include <vector>

namespace {

const float PI = 3.14159265358979f;

// Major radius of the centre ring and half-width of the strip.
const float kRadius    = 0.75f;
const float kHalfWidth = 0.30f;

const int kAroundSteps = 200;   // around the ring; needs to be fine, the twist
                                // makes a coarse mesh visibly faceted
const int kAcrossSteps = 8;     // across the width

// The surface itself. u runs once around the ring, v across the strip.
//
//   x = (R + v*cos(u/2)) * cos u
//   y =      v*sin(u/2)
//   z = (R + v*cos(u/2)) * sin u
//
// The u/2 is the whole trick: after a full lap (u goes 0 -> 2pi) the
// cross-section has rotated only half a turn, so the strip joins back to
// itself inverted.
glm::vec3 surfacePoint(float u, float v) {
    float halfU = u * 0.5f;
    float radial = kRadius + v * std::cos(halfU);
    return glm::vec3(radial * std::cos(u),
                     v * std::sin(halfU),
                     radial * std::sin(u));
}

} // namespace

Mobius::Mobius(float x, float y, float z, float scale, int colorIndex, int id)
    : Shape(x, y, z, scale, colorIndex, id), VAO(0), VBO(0), EBO(0), indexCount(0) {
    shapeType = "Mobius Strip";
    setupMobius();
}

Mobius::~Mobius() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mobius::setupMobius() {
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    vertices.clear();
    normals.clear();
    faces.clear();

    // Normals come from the cross product of the two surface tangents,
    // evaluated by central difference. The analytic partials of this surface
    // are unpleasant, and the numeric ones are accurate to well past what the
    // shading needs.
    const float h = 1e-3f;

    for (int i = 0; i <= kAroundSteps; ++i) {
        float u = 2.0f * PI * static_cast<float>(i) / kAroundSteps;

        for (int j = 0; j <= kAcrossSteps; ++j) {
            float v = -kHalfWidth + 2.0f * kHalfWidth *
                      static_cast<float>(j) / kAcrossSteps;

            vertices.push_back(surfacePoint(u, v));

            glm::vec3 du = surfacePoint(u + h, v) - surfacePoint(u - h, v);
            glm::vec3 dv = surfacePoint(u, v + h) - surfacePoint(u, v - h);
            glm::vec3 n = glm::cross(du, dv);

            float len = glm::length(n);
            normals.push_back(len > 1e-9f ? n / len : glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    // u runs once around the loop, v across the width -- the surface's own
    // parameters, which is the natural map for any parametric surface.
    const std::vector<glm::vec2> gridUVs =
        TexCoords::parametricGrid(kAroundSteps, kAcrossSteps);
    texCoords.clear();

    const int rowLength = kAcrossSteps + 1;
    for (int i = 0; i < kAroundSteps; ++i) {
        for (int j = 0; j < kAcrossSteps; ++j) {
            int a = i * rowLength + j;
            int b = a + 1;
            int c = a + rowLength;
            int d = c + 1;

            faces.push_back({a, c, b});
            faces.push_back({b, c, d});
        }
    }

    for (size_t i = 0; i < faces.size(); ++i) {

        glm::vec3 color = (colorIndex == 31)
            ? glm::vec3(customColor[0], customColor[1], customColor[2])
            : glm::vec3(colorPresets[colorIndex].color[0],
                        colorPresets[colorIndex].color[1],
                        colorPresets[colorIndex].color[2]);

        for (int j = 0; j < 3; ++j) {
            int vertexIndex = faces[i][j];
            const glm::vec3& position = vertices[vertexIndex];
            const glm::vec3& normal   = normals[vertexIndex];

            vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
            vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
            vertexData.insert(vertexData.end(), {color.r, color.g, color.b});

            // Expanded per corner alongside the vertex data: indexData below is
            // sequential, so attribute 3 has to be the same length as this
            // buffer, not the length of the unique-vertex list.
            texCoords.push_back(vertexIndex < static_cast<int>(gridUVs.size())
                                ? gridUVs[static_cast<size_t>(vertexIndex)]
                                : glm::vec2(0.0f));
        }

        indexData.insert(indexData.end(), {static_cast<unsigned int>(i * 3),
                                            static_cast<unsigned int>(i * 3 + 1),
                                            static_cast<unsigned int>(i * 3 + 2)});
    }

    indexCount = static_cast<int>(indexData.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float),
                 vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int),
                 indexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // UVs in their own buffer on attribute 3, while the VAO is still bound.
    uploadTexCoords();
    glBindVertexArray(0);
}

void Mobius::draw(GLuint shaderProgram) {
    glUseProgram(shaderProgram);

    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) glUniform1i(lightingLoc, 1);

    applyTransform(shaderProgram);

    GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
    if (colorLoc != -1) {
        glUniform3fv(colorLoc, 1,
                     (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
    }

    // A one-sided surface has no back to cull, and the viewer sees both faces
    // of the strip at once, so culling must stay off here.
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
