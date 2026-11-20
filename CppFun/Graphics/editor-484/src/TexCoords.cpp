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
        ((latSegments - 1) - static_cast<float>(i)) / static_cast<float>(latSegments - 1);
    // xz moves on u axis.
    // y moves on v axis.
    // float const v = glm::cos(phi);
    float const v = phi;
    // std::cout << "--\nv = " << v << std::endl;
    for (unsigned j = 0; j < lonSegments; ++j) {
      float const theta = 
        ((lonSegments - 1) - static_cast<float>(j)) / static_cast<float>(lonSegments - 1); float const u = theta; uvPerVertex.push_back(glm::vec2(u, v));

      //std::cout << "u = " << u << std::endl;
    }
  }
  
  return uvPerVertex;
}

// TODO(textures): generate one UV per cube triangle corner
// Three per face, in the order the corners appear in `faces`. Project
// each vertex onto the two axes the face normal is NOT aligned with.
// faceNormals is indexed faceNormals[i / 2]: two triangles per face.
//  return vector of uvs for each non-unique vertex.
std::vector<glm::vec2> cube(const std::vector<glm::vec3> &vertices,
                            const std::vector<std::vector<int>> &faces,
                            const std::vector<glm::vec3> &faceNormals) {
  // AABB projection. See TexCoordsGiven.cpp's planarPerFace 
  // function for reference.
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
  for (unsigned k = 0; k < 3; ++k) if (span[k] < 1e-6f) span[k] = 1.0f;

  std::cout << "\nCube AABB box Min "
    << boxMin.x <<", "
    << boxMin.y << ", "
    << boxMin.z << std::endl;
  std::cout << "\nCube AABB box Max "
    << boxMax.x <<", "
    << boxMax.y << ", "
    << boxMax.z << std::endl;
  std::cout << "\nCube AABB span "
    << span.x <<", "
    << span.y << ", "
    << span.z << std::endl;

  std::vector<glm::vec2> uvs; 
  uvs.reserve(faces.size() * 3); // uv for each vertex

  for(unsigned i = 0; i < 2; i+=2) {
    std::vector<int> const face = faces.at(i);
 
    glm::vec3 normal = faceNormals.at(face.at(0));

    float const ax = std::fabs(normal.x),
                ay = std::fabs(normal.y),
                az = std::fabs(normal.z);

    std::cout << "\nCube TexCoords"
      << "\nnormal.x = " << normal.x
      << "\nnormal.y = " << normal.y
      << "\nnormal.z = " << normal.z << std::endl;

    int uAxis, vAxis;
    if (az >= ax && az >= ay) { uAxis = 1; vAxis = 0; std::cout << "XY" << std::endl; }
    else if (ax > ay)         { uAxis = 2; vAxis = 1; std::cout << "YZ" << std::endl; }
    else                      { uAxis = 0; vAxis = 2; std::cout << "XZ" << std::endl; }

    uvs.push_back(glm::vec2(0, 0));
    uvs.push_back(glm::vec2(1, 0));
    uvs.push_back(glm::vec2(1, 1));

    glm::vec3 const position = vertices.at(faces.at(0).at(0));
    std::cout
      << "\n\nx pos: " << position.x
      << "\ny pos: " << position.y
      << "\nz pos: " << position.z
      << "\nUVs = "
      << uvs.at(uvs.size()-1).x
      << ", "
      << uvs.at(uvs.size()-1).y
      << std::endl;
    /*
    uvs.push_back(glm::vec2(1, 1));
    uvs.push_back(glm::vec2(0, 1));
    uvs.push_back(glm::vec2(0, 0));
*/
// STATICALLY PUSH BACK UV VALUES. SEE IF THAT WORKS. just for the first face, then second face. See what happens..
//
/*
    // tri 1
    for (unsigned j = 0; j < 3; ++j) {
      // compute uv bounds. 
      // uvs applied with respect to smaller-aligned axes.
      // span captures the axis-aligned bounds of the model  
      // normalizes vertex position wrt U/V axes of boxMin and span.

      glm::vec3 const position = vertices.at(faces.at(i).at(j)); 
*/
/*      uvs.push_back(glm::vec2(
        (position[uAxis] - boxMin[uAxis]) / span[uAxis],
        (position[vAxis] - boxMin[vAxis]) / span[vAxis]
      ));
*/
/*
    glm::vec2 newUV;
      switch (j) {
        case 0: newUV = glm::vec2(0, 0); break;
        case 1: newUV = glm::vec2(1, 0); break;
        case 2: newUV = glm::vec2(1, 1); break;
      }
      uvs.push_back(newUV);

      std::cout
        << "\n\nx pos: " << position.x
        << "\ny pos: " << position.y
        << "\nz pos: " << position.z
        << "\nUVs = "
        << uvs.at(uvs.size()-1).x
        << ", "
        << uvs.at(uvs.size()-1).y
        << "\nj = " << j << std::endl;
    }
*/
/*
    // tri 2
    for (unsigned j = 0; j < 3; ++j) {
      glm::vec3 const position = vertices.at(faces.at(i+1).at(j)); 
      glm::vec2 newUV;
      switch (j) {
        case 0: newUV = glm::vec2(1, 1); break;
        case 1: newUV = glm::vec2(0, 1); break;
        case 2: newUV = glm::vec2(0, 0); break;
      }
      uvs.push_back(newUV);

      std::cout
        << "\n\nx pos: " << position.x
        << "\ny pos: " << position.y
        << "\nz pos: " << position.z
        << "\nUVs = "
        << uvs.at(uvs.size()-1).x
        << ", "
        << uvs.at(uvs.size()-1).y
        << "\nj = " << j << std::endl;

    }
*/
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

  if (valid() == false) {
    std::cout << "Invalid texture for raytrace sample." << std::endl; 
    return glm::vec3(1.0f);
  }

  //-///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // MOTIVATION: Linearly interpolate between pixel colors based on the ray to pixel position delta.
  //
  //              rayDelta = rayPosition - pixelPosition
  //              colorStrength = pixelColor * rayDelta
  //              finalColor = Sum of pixels' color strengths
  //
  //      First, we must convert uv positions to xy position.
  //      Then, we can derive relative the rayDelta.
  //      Finally, we can derive and sum the color strengths.
  //
  //                        ---
  //
  // A pixel's uv position is proportionally equal to that pixel's x, y position.
  //    That means (u, v) * (width, height) = (x, y) / (width, height)
  //    where width applies to u and x and height applies to v and y.
  //
  // Each pixel has a height and width of 1.
  //    For convenience, we shift x, y positions by -0.5f such that each pixel's center is divisible by 1.0f.
  //    A pixel's center is equal to it's position.
  //    For example, in a pixel quad,
  //        pixel A's position is (3, 3),
  //        pixel B's position is (4, 3),
  //        pixel C's position is (3, 4),
  //        pixel D's position is (4, 4)
  //
  // Since each pixel's x or y position is divisible by 1, we can use floor() and ceil() to find
  //    which 4 pixels are closest to the ray's x, y position.
  //
  // We want to combine pixel colors relative to the ray's position.
  //    In other words, relative color is function of relative position.
  // By getting the distance from one pixel to the ray, we can derive its and all the other pixel's relative color strength.
  //    tx = (rayPosition.x - topLeftPixelPosition.x)
  //    tx' = 1 - tx;
  //    ty = (rayPosition.y - topLeftPixelPosition.y)
  //    ty' = 1 - ty;
  //
  //    tx and ty are bounded to [0, 1].
  //
  //    tx -- right pixels. Greater the distance, the greater right pixels' strength.
  //    tx' -- left pixels. If a ray's position is (0.9, 0.9), (tx', ty') = (0.1, 0.1).
  //    ty -- bottom pixels. Greater the distance, the greater bottom pixels' strength.
  //    ty' -- top pixels. If a ray's position is (0.4, 0.4), (tx', ty') = (0.6, 0.6).
  //
  // Each pixel's color and position is contained in the 'pixels' vector. 
  // A pixel's x,y position can be derived from its index using a texture's width and height.
  //    You can also derive the index given an x,y position and the texture's width and height.
  // A pixel's color is stored at that pixel's index in the 'pixels' vector.
  // Get each pixel's color using it's x, y position and the texture's width and height.
  //
  // To get the final color, multiply each pixel's color by the appropriate a*b combo of t's and add them together.
  //    topLeftColor * tx' * ty' +
  //    topRightColor * tx * ty' +
  //    bottomLeftColor * tx' * ty +
  //    bottomRightColor * tx * ty

  // 1. Derive ray's x, y position from uv position.

  // x >= 0
  if (u < 0) u *= -1.0f;
  if (v < 0) v *= -1.0f;
  // x < 1.0f
  u -= int(u / 1.0f);
  v -= int(v / 1.0f);
 
  // convert to pixel coordinates. [-0.5 + -0.5*n, 0.5 + 0.5*n] for each pixel n. n >= 0. n%1.0f = 0
  // u, v is texture-size scale.
  // x, y converts u,v to pixel resolution.
  float const rayX = u * width - 0.5f; 
  float const rayY = v * height - 0.5f;

  // 2. Derive nearest pixels' x, y positions from ray's x, y position.

  int x0 = std::floor(rayX);
  int y0 = std::floor(rayY);
  int x1 = x0 + 1.0f;
  int y1 = y0 + 1.0f;

  // avoid out-of-bounds edge-case
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= width) x1 = width;
  if (y1 >= height) y1 = height;

  std::cout << 
    "\nTexture width, height: " << width << ", " << height <<
    "\nTexture's pixels array size" << pixels.size() << 
    std::endl;

  // 3. Derive color strength scalar based on delta

  // Derive x, y distances. They are bounded [0, 1]
  float const xDelta = rayX - x0;
  float const yDelta = rayY - y0;
  float const xDeltaPrime = 1 - xDelta;
  float const yDeltaPrime = 1 - yDelta; 

  // Derive scalar for each pixel
  float const tTopLeft  = xDeltaPrime * yDeltaPrime;
  float const tTopRight = xDelta * yDeltaPrime;
  float const tBotLeft  = xDeltaPrime * yDelta;
  float const tBotRight = xDelta * yDelta;

  // 4. Get pixel colors by deriving pixel indices from pixel positions.

  // Pixel Position by Index
  // 0 = top-left. 1 = top-right. 2 = bottom-left. 3 = bottom-right.
  std::vector<int> const xpixelPos = { x0, x1, x0, x1};
  std::vector<int> const ypixelPos = { y0, y0, y1, x1};

  // Extract pixel color for each pixel based on indices.
  // width is n pixels * m values per pixel.
  // each unsigned char is a 0-255 integer
  std::vector<glm::vec3> pixelColors;
  for (size_t i = 0; i < xpixelPos.size(); ++i) { 
    // pixels array has 4 unsigned chars per pixel. One uchar for each R, G, B, and A values.
    int offset = xpixelPos.at(i) * 4
                  + ypixelPos.at(i) * width * 4;
    // get RGB color values at index offset + 0, +1, +2
    pixelColors.push_back(
      glm::vec3(
        pixels.at(offset    ) / 255,
        pixels.at(offset + 1) / 255,
        pixels.at(offset + 2) / 255
    ));
  }

  // 5. Sum averaged colors.

  pixelColors.at(0) *= tTopLeft;
  pixelColors.at(1) *= tTopRight;
  pixelColors.at(2) *= tBotLeft;
  pixelColors.at(3) *= tBotRight;

  glm::vec3 color = glm::vec3(0.0f);
  for (unsigned i = 0; i < pixelColors.size(); ++i) {
    color += pixelColors.at(i);
  }

  return color; 
}
