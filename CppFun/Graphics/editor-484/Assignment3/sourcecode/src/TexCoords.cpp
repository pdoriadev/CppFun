// =============================================================================
// Texture coordinate generation and sampling
// =============================================================================
// Student work. See include/TexCoords.h for why this is its own translation
// unit and what stays on the given side.

#include "TexCoords.h"
#include "TextureCache.h"

#include <cmath>
#include <algorithm>

namespace TexCoords {

std::vector<glm::vec2> sphere(unsigned latSegments, unsigned lonSegments) {
    // TODO(textures): generate one UV per sphere vertex
    // Walk the same loop bounds Sphere::setupSphere uses, so entry i here
    // matches vertex i there. Remember v runs bottom-up for OpenGL.
    (void)latSegments; (void)lonSegments;
    return std::vector<glm::vec2>();
}

std::vector<glm::vec2> cube(const std::vector<glm::vec3>& vertices,
                            const std::vector<std::vector<int> >& faces,
                            const std::vector<glm::vec3>& faceNormals) {
    // TODO(textures): generate one UV per cube triangle corner
    // Three per face, in the order the corners appear in `faces`. Project
    // each vertex onto the two axes the face normal is NOT aligned with.
    // faceNormals is indexed faceNormals[i / 2]: two triangles per face.
    (void)vertices; (void)faces; (void)faceNormals;
    return std::vector<glm::vec2>();
}

// Cylindrical projection, one UV per triangle corner.
//
// For anything shaped roughly like a solid of revolution but WITHOUT a
// generating parameterisation to reuse -- the teapot above all, whose vertex
// data arrives as a bare triangle soup from a table of Bezier patches.
//
// u is the angle round the Y axis, v the height, both normalised by the mesh's
// own bounds.
std::vector<glm::vec2> cylindricalPerCorner(const std::vector<glm::vec3>& vertices,
                                            const std::vector<std::vector<int> >& faces) {
    // TODO(textures): cylindrical projection with the wrap seam repaired
    // Project every triangle corner onto a cylinder about the Y axis: u is
    // the angle round it (atan2(z, x), remapped from -pi..pi into 0..1), v is
    // the height normalised by the mesh's own Y bounds. Return three UVs per
    // face, in corner order.
    //
    // Then repair the wrap seam, which is the part worth the effort. A
    // triangle straddling the back of the mesh gets corners at u = 0.98 and
    // u = 0.02; interpolated across the face that runs the LONG way round and
    // squeezes the whole image, mirrored, into one triangle -- a bright
    // vertical scar down the back. Detect it (the u range across a triangle
    // exceeds half a turn) and push the low corners past 1.0 so the span is
    // short again; wrapping in the sampler puts them back.
    //
    // That repair is only expressible per corner, which is why this returns
    // per-corner UVs: one point in space needs two different u values
    // depending on which triangle is asking.
    (void)vertices; (void)faces;
    return std::vector<glm::vec2>();
}

} // namespace TexCoords

glm::vec3 Texture::sample(float u, float v) const {
    // TODO(textures): bilinear sample in normalised coordinates, wrapping outside [0, 1]
    // Wrap u and v into [0, 1), find the four surrounding texels, and
    // blend them. Texel CENTRES sit at (i + 0.5) / size, which is why the
    // -0.5 below matters: without it the image is offset by half a texel.
    (void)u; (void)v;
    return glm::vec3(1.0f);
}
