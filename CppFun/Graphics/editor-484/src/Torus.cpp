// =============================================================================
// Torus -- the worked procedural example for Assignment 2
// =============================================================================
// Given. Commented for reading, not for brevity: this file is the thing the
// handout tells students to read before writing the Sphere. See Torus.h.

#include "Torus.h"
#include "TexCoords.h"

#include <cmath>
#include <vector>

namespace {

const float PI = 3.14159265358979f;

// R is the distance from the centre of the hole to the centre of the tube;
// r is the radius of the tube itself. R + r = 0.5 keeps the whole shape inside
// the one-unit convention every generator in this editor follows, so it can be
// dropped into a scene beside a cube without dwarfing it.
const float kMajorRadius = 0.35f;   // R
const float kMinorRadius = 0.15f;   // r

// Segments around each of the two circles. These are the resolution knobs: too
// few and the silhouette is visibly polygonal, too many and every downstream
// pass (the ray tracer especially) pays for it.
const int kMajorSteps = 48;   // around the ring
const int kMinorSteps = 24;   // around the tube

// A point on the surface, in terms of two angles.
//
//   u  goes around the RING   (0 .. 2pi)
//   v  goes around the TUBE   (0 .. 2pi)
//
// Read it as: walk out to distance R along direction u, then step r away from
// that centreline in a direction that itself rotates with v.
//
//   x = (R + r*cos v) * cos u
//   y =      r*sin v
//   z = (R + r*cos v) * sin u
//
// The bracketed term is the distance from the Y axis: it grows and shrinks as v
// carries you around the outside and the inside of the tube. That is the whole
// shape.
glm::vec3 surfacePoint(float u, float v) {
    const float radial = kMajorRadius + kMinorRadius * std::cos(v);
    return glm::vec3(radial * std::cos(u),
                     kMinorRadius * std::sin(v),
                     radial * std::sin(u));
}

} // namespace

Torus::Torus(float x, float y, float z, float scale, int colorIndex, int id)
    : Shape(x, y, z, scale, colorIndex, id), VAO(0), VBO(0), EBO(0), indexCount(0) {
    shapeType = "Torus";
    setupTorus();
}

Torus::~Torus() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Torus::setupTorus() {
    vertices.clear();
    normals.clear();
    faces.clear();

    // -----------------------------------------------------------------------
    // STEP 1: the vertices.
    //
    // A grid over (u, v). Note the <= in both loops: the ring at u = 2pi is the
    // SAME ring as u = 0, and it is emitted twice on purpose.
    //
    // Emitting it twice costs one extra ring of vertices and buys a much
    // simpler step 2 -- every cell is (i, j) to (i+1, j+1) with no wraparound
    // arithmetic -- and it is what makes a seamless texture possible in
    // Assignment 5, because the duplicate ring can carry u = 1 where the
    // original carries u = 0. A single point in space, two different texture
    // coordinates. The alternative, wrapping the indices with a modulo, saves
    // the vertices and makes both of those harder.
    //
    // THE ORDER MATTERS AND IS THE CONTRACT for step 2: i is the outer loop,
    // j the inner, so the vertex for grid point (i, j) is at index
    // i * (kMinorSteps + 1) + j. Step 2 does nothing but rely on that.
    // -----------------------------------------------------------------------
    for (int i = 0; i <= kMajorSteps; ++i) {
        const float u = 2.0f * PI * static_cast<float>(i) / kMajorSteps;

        for (int j = 0; j <= kMinorSteps; ++j) {
            const float v = 2.0f * PI * static_cast<float>(j) / kMinorSteps;
            vertices.push_back(surfacePoint(u, v));
        }
    }

    // -----------------------------------------------------------------------
    // STEP 2: the faces.
    //
    // Each CELL of the grid -- the little quad between (i, j), (i+1, j),
    // (i+1, j+1) and (i, j+1) -- becomes two triangles. Note the loops stop one
    // short of the ends: there are kMajorSteps cells between kMajorSteps + 1
    // rings, the same way a fence with n panels has n + 1 posts. Running these
    // loops to <= is the single most common way to index past the end of
    // `vertices`.
    //
    // The corner order within each triangle is what sets the WINDING, and the
    // winding is what decides which way calculateNormals() will point the
    // normal. Both triangles below go round the cell the same way, so both
    // normals come out on the same side of the surface. Reverse one and that
    // face will be lit from inside while its neighbour is lit from outside.
    // -----------------------------------------------------------------------
    const int stride = kMinorSteps + 1;

    for (int i = 0; i < kMajorSteps; ++i) {
        for (int j = 0; j < kMinorSteps; ++j) {
            const int a = i * stride + j;            // (i,   j  )
            const int b = (i + 1) * stride + j;      // (i+1, j  )
            const int c = (i + 1) * stride + j + 1;  // (i+1, j+1)
            const int d = i * stride + j + 1;        // (i,   j+1)

            std::vector<int> t1;
            t1.push_back(a); t1.push_back(b); t1.push_back(c);
            faces.push_back(t1);

            std::vector<int> t2;
            t2.push_back(a); t2.push_back(c); t2.push_back(d);
            faces.push_back(t2);
        }
    }

    // -----------------------------------------------------------------------
    // STEP 3: normals, from the geometry that was just built.
    //
    // A torus has an analytic normal -- it points straight out from the centre
    // of the tube -- and using it would give a slightly smoother result. The
    // cross product is used anyway, because that is what the assignment asks
    // for and this file is meant to model the answer.
    // -----------------------------------------------------------------------
    calculateNormals();

    // -----------------------------------------------------------------------
    // STEP 4: hand it to OpenGL. Given, and the same in every shape class: the
    // interleaved buffer is position(3) + normal(3) + colour(3), stride 9.
    //
    // Note what this loop does with the arrays above: it emits three FRESH
    // vertices per face rather than reusing the shared ones, so the vertex
    // count that reaches the GPU is faces.size() * 3, not vertices.size(). That
    // is why the per-face normal from step 3 can be attached to each corner --
    // and it is the detail that will matter in Assignment 5, when texture
    // coordinates have to line up with it.
    // -----------------------------------------------------------------------
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    const glm::vec3 colour = (colorIndex == 31)
        ? glm::vec3(customColor[0], customColor[1], customColor[2])
        : glm::vec3(colorPresets[colorIndex].color[0],
                    colorPresets[colorIndex].color[1],
                    colorPresets[colorIndex].color[2]);

    for (size_t f = 0; f < faces.size(); ++f) {
        const glm::vec3& n = normals[f];

        for (int k = 0; k < 3; ++k) {
            const glm::vec3& p = vertices[faces[f][k]];
            vertexData.push_back(p.x); vertexData.push_back(p.y); vertexData.push_back(p.z);
            vertexData.push_back(n.x); vertexData.push_back(n.y); vertexData.push_back(n.z);
            vertexData.push_back(colour.r); vertexData.push_back(colour.g); vertexData.push_back(colour.b);
            indexData.push_back(static_cast<unsigned int>(indexData.size()));
        }
    }

    indexCount = static_cast<int>(indexData.size());

    // Texture coordinates: the grid parameters ARE the map, expanded per corner
    // to match the buffer above. Given here; generating them is Assignment 5.
    {
        const std::vector<glm::vec2> gridUVs =
            TexCoords::parametricGrid(kMajorSteps, kMinorSteps);

        texCoords.clear();
        texCoords.reserve(faces.size() * 3);
        for (size_t f = 0; f < faces.size(); ++f) {
            for (int k = 0; k < 3; ++k) {
                const int vi = faces[f][k];
                texCoords.push_back(vi < static_cast<int>(gridUVs.size())
                                    ? gridUVs[vi] : glm::vec2(0.0f));
            }
        }
    }

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

    uploadTexCoords();
    glBindVertexArray(0);
}

void Torus::draw(GLuint shaderProgram) {
    glUseProgram(shaderProgram);

    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) glUniform1i(lightingLoc, 1);

    applyTransform(shaderProgram);
    applyMaterial(shaderProgram);

    GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
    if (colorLoc != -1) {
        glUniform3fv(colorLoc, 1,
                     (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
