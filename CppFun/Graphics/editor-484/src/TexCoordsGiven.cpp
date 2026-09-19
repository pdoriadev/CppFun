// =============================================================================
// TexCoords -- the GIVEN projections
// =============================================================================
// planarPerFace() and parametricGrid() are given. They live here, in a file of
// their own, and NOT beside the student's sphere()/cube()/cylindricalPerCorner()
// in TexCoords.cpp.
//
// WHY THE SPLIT. planarPerFace() picks a projection plane by finding the
// dominant axis of the face normal and dropping that coordinate:
//
//     if (az >= ax && az >= ay)  { uAxis = 0; vAxis = 1; }
//     else if (ax >= ay)         { uAxis = 2; vAxis = 1; }
//     else                       { uAxis = 0; vAxis = 2; }
//
// which is, branch for branch and axis for axis, the answer to cube(). A
// student with a TODO in cube() and this function fifteen lines below it has
// been handed the algorithm. Same failure mode as the two recursive helpers
// that used to sit outside the markers in Skinning.cpp -- a helper in the
// student's own file that IS the assignment. See ASSIGNMENTS.md, leak audit.
//
// Nothing about the behaviour changes: same namespace, same signatures, same
// callers. Only the translation unit differs, and `textures_SRC` swaps
// TexCoords.cpp alone.

#include "TexCoords.h"

#include <cmath>
#include <algorithm>

namespace TexCoords {


std::vector<glm::vec2> planarPerFace(const std::vector<glm::vec3>& vertices,
                                     const std::vector<std::vector<int> >& faces) {
    std::vector<glm::vec2> uvs;
    uvs.reserve(faces.size() * 3);
    if (vertices.empty()) return uvs;

    // Bounding box, so the projection fills 0..1 whatever scale the mesh is
    // authored at. A pyramid spanning -1..1 and a cube spanning -0.5..0.5 then
    // both map the whole image onto each face.
    glm::vec3 mn = vertices[0], mx = vertices[0];
    for (size_t i = 1; i < vertices.size(); ++i) {
        mn = glm::min(mn, vertices[i]);
        mx = glm::max(mx, vertices[i]);
    }
    glm::vec3 span = mx - mn;
    for (int k = 0; k < 3; ++k) if (span[k] < 1e-6f) span[k] = 1.0f;  // flat mesh

    for (size_t f = 0; f < faces.size(); ++f) {
        if (faces[f].size() < 3) continue;

        const glm::vec3& a = vertices[static_cast<size_t>(faces[f][0])];
        const glm::vec3& b = vertices[static_cast<size_t>(faces[f][1])];
        const glm::vec3& c = vertices[static_cast<size_t>(faces[f][2])];

        // The face's own normal. Taking it from the geometry rather than a
        // normals array means this works for any mesh, including ones whose
        // normals are per vertex or absent.
        const glm::vec3 n = glm::cross(b - a, c - a);

        // Project onto the two axes the normal is LEAST aligned with. Using
        // the dominant axis would collapse the face to a line.
        const float ax = std::fabs(n.x), ay = std::fabs(n.y), az = std::fabs(n.z);
        int uAxis, vAxis;
        if (az >= ax && az >= ay)      { uAxis = 0; vAxis = 1; }
        else if (ax >= ay)             { uAxis = 2; vAxis = 1; }
        else                           { uAxis = 0; vAxis = 2; }

        for (int j = 0; j < 3; ++j) {
            const glm::vec3& p = vertices[static_cast<size_t>(faces[f][j])];
            uvs.push_back(glm::vec2((p[uAxis] - mn[uAxis]) / span[uAxis],
                                    (p[vAxis] - mn[vAxis]) / span[vAxis]));
        }
    }
    return uvs;
}

std::vector<glm::vec2> parametricGrid(unsigned uSteps, unsigned vSteps,
                                      bool flipU, bool flipV) {
    std::vector<glm::vec2> uvs;
    uvs.reserve(static_cast<size_t>(uSteps + 1) * (vSteps + 1));

    // j varies fastest, matching every generator that uses this shape of loop.
    for (unsigned i = 0; i <= uSteps; ++i) {
        for (unsigned j = 0; j <= vSteps; ++j) {
            float u = static_cast<float>(i) / static_cast<float>(uSteps);
            float v = static_cast<float>(j) / static_cast<float>(vSteps);
            if (flipU) u = 1.0f - u;
            if (flipV) v = 1.0f - v;
            uvs.push_back(glm::vec2(u, v));
        }
    }
    return uvs;
}

} // namespace TexCoords
