// =============================================================================
// Texture coordinate generation and sampling
// =============================================================================
// Student work. See include/TexCoords.h for why this is its own translation
// unit and what stays on the given side.

#include "../include/TexCoords.h"
#include "../include/TextureCache.h"

#include <algorithm>
#include <cmath>
#include <glm/ext/vector_float2.hpp>
#include <iostream>

#define _USE_MATH_DEFINES // PI
#include <math.h>

namespace TexCoords {

std::vector<glm::vec2> sphere(unsigned latSegments, unsigned lonSegments) {
  // generate one UV per sphere vertex
  // Walk the same loop bounds Sphere::setupSphere uses, so entry i here
  // matches vertex i there. Remember v runs bottom-up for OpenGL.
  std::vector<glm::vec2> uvPerVertex;
  uvPerVertex.reserve(latSegments * lonSegments);

  std::cout << std::endl;
  for (unsigned i = 0; i < latSegments; ++i) {
    float const phi =
        static_cast<float>(i) / static_cast<float>(latSegments - 1);
    // xz moves on u axis.
    // y moves on v axis.
    // float const v = glm::cos(phi);
    float const v = phi;
    // std::cout << "--\nv = " << v << std::endl;
    for (unsigned j = 0; j < lonSegments; ++j) {
      float const theta = static_cast<float>(j) / static_cast<float>(latSegments - 1);
      float const u = theta;
      uvPerVertex.push_back(glm::vec2(u, v));

      //std::cout << "u = " << u << std::endl;
    }
  }
  
  return uvPerVertex;
}

std::vector<glm::vec2> cube(const std::vector<glm::vec3> &vertices,
                            const std::vector<std::vector<int>> &faces,
                            const std::vector<glm::vec3> &faceNormals) {
  // TODO(textures): generate one UV per cube triangle corner
  // Three per face, in the order the corners appear in `faces`. Project
  // each vertex onto the two axes the face normal is NOT aligned with.
  // faceNormals is indexed faceNormals[i / 2]: two triangles per face.
  
  // AABB projection. See planarProjection function for reference.
  // return vector of uvs for each non-unique vertex.
  if (vertices.size() == 0) return {};
  if (faces.size() == 0) return {};
  if (faceNormals.size() == 0) return {};
  
  glm::vec3 boxMin(INFINITY, INFINITY, INFINITY);
  glm::vec3 boxMax(-INFINITY, -INFINITY, -INFINITY);
  for (unsigned j = 0; j < vertices.size(); ++j) {
    boxMin = glm::min(boxMin, vertices.at(j));
    boxMax = glm::max(boxMax, vertices.at(j));
  }

  glm::vec3 span = boxMax - boxMin;

  // ensures a minimum width greater than 1e-6f. Avoids divide by 0.
  for (unsigned s = 0; s < 3; ++s) if (span[s] < 1e-6f) span[s] = 1.0f;

  std::vector<glm::vec2> uvs; 
  uvs.reserve(faces.size() * 3); // uv for each vertex

  for(unsigned i = 0; i < faces.size(); ++i) {
    std::vector<int> const face = faces.at(i);
    
    glm::vec3 normal = faceNormals.at(face.at(0));

    float const ax = std::fabs(normal.x), 
                ay = std::fabs(normal.y), 
                az = std::fabs(normal.z);

    std::cout <<
      "\nCube TexCoords - \nnormal.x = " << normal.x 
      << "\nnormal.y = "<< normal.y 
      << "\nnormal.z = "<< normal.z << std::endl;
    int uAxis, vAxis;
    if (az >= ax && az >= ay) { uAxis = 0; vAxis = 1; std::cout << "XY" << std::endl; }
    else if (ax > ay)         { uAxis = 1; vAxis = 2; std::cout << "YZ" << std::endl; }
    else                      { uAxis = 0; vAxis = 2; std::cout << "XZ" << std::endl; }
    
    for (unsigned j = 0; j < 3; ++j) {
      // compute uv bounds. 
      // uvs applied with respect to smaller-aligned axes.
      // span captures the axis-aligned bounds of the model  
      // normalizes vertex position wrt U/V axes of boxMin and span.

      glm::vec3 const position = vertices.at(faces.at(i).at(j)); 
      uvs.push_back(glm::vec2(
        position[uAxis] - boxMin[uAxis] / span[uAxis],
        position[vAxis] - boxMin[vAxis] / span[vAxis]
      ));
    }
  }

  return uvs; 
  
}

// Cylindrical projection, one UV per triangle corner.
//
// For anything shaped roughly like a solid of revolution but WITHOUT a
// generating parameterisation to reuse -- the teapot above all, whose vertex
// data arrives as a bare triangle soup from a table of Bezier patches.
//
// u is the angle round the Y axis, v the height, both normalised by the mesh's
// own bounds.
std::vector<glm::vec2>
cylindricalPerCorner(const std::vector<glm::vec3> &vertices,
                     const std::vector<std::vector<int>> &faces) {
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
  (void)vertices;
  (void)faces;
  return std::vector<glm::vec2>();
}

} // namespace TexCoords

glm::vec3 Texture::sample(float u, float v) const {
  // TODO(textures): bilinear sample in normalised coordinates, wrapping outside
  // [0, 1] Wrap u and v into [0, 1), find the four surrounding texels, and
  // blend them. Texel CENTRES sit at (i + 0.5) / size, which is why the
  // -0.5 below matters: without it the image is offset by half a texel.

  glm::vec3 color;
  // x >= 0
  if (u < 0) u *= -1.0f;
  if (v < 0) v *= -1.0f;
  // x < 1.0f
  u -= int(u / 1.0f);
  v -= int(v / 1.0f);
  
  std::cout << std::endl;
  for (unsigned i = 0; i < pixels.size(); ++i) {
    std::cout << pixels.at(i) << std::endl;
  }

  /* Are we extracting the color from pixels? 
   * Maybe we look at the size of  bj
   *
   * */ 

  return glm::vec3();
}
