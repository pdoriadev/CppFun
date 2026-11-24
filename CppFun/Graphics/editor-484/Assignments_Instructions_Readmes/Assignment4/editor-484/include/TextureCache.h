#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#include <string>
#include <vector>
#include <map>

#include "glad/glad.h"
#include <glm/glm.hpp>

// =============================================================================
// TextureCache -- decoded images, shared by path
// =============================================================================
// A brick wall on six cubes should be decoded once and uploaded once. The
// cache is keyed by resolved file path and hands back both halves of a
// texture:
//
//   handle   the GL texture object, for the rasterized viewport
//   pixels   the decoded bytes still on the host, for the ray tracer
//
// Keeping the pixels is not redundancy. RayTracer::shade() runs entirely on
// the CPU and cannot read a GL texture; without a host copy, a textured scene
// would render one way in the viewport and another way in a trace. See
// tests/bench.cpp for why the tracer never touches the GPU.
//
// Material stores the PATH, not the handle, so SceneIO round-trips a string
// and the cache resolves it -- the same arrangement the scene format already
// uses for mesh paths.
//
// This whole file is given to students. Image decoding is not the lesson;
// parameterisation and sampling are.

struct Texture {
    GLuint                     handle;   // 0 if the GL upload has not happened
    int                        width;
    int                        height;
    std::vector<unsigned char> pixels;   // RGBA8, row 0 at the BOTTOM (see below)

    Texture() : handle(0), width(0), height(0) {}

    bool valid() const { return width > 0 && height > 0 && !pixels.empty(); }

    // Bilinear sample in normalised coordinates, wrapping outside [0, 1].
    // Used by the ray tracer; the viewport samples through GL instead.
    glm::vec3 sample(float u, float v) const;
};

namespace TextureCache {

// Decodes if this path has not been seen, otherwise returns the cached entry.
// Returns null and appends to lastError() if the file cannot be read.
//
// Rows are flipped on load so that v = 0 is the BOTTOM of the image, which is
// what OpenGL expects and what makes the viewport and the tracer agree.
const Texture* get(const std::string& path);

// Uploads to GL on first use and returns the handle, or 0. Separate from get()
// because the ray tracer needs the pixels without a GL context, and the tests
// run headless.
GLuint handleFor(const std::string& path);

// Frees the GL objects. Call before the context goes away.
void releaseAll();

// Number of distinct paths currently held. Tests assert on this to prove the
// cache is actually deduplicating rather than decoding six times.
size_t size();

const std::string& lastError();

} // namespace TextureCache

#endif // TEXTURECACHE_H
