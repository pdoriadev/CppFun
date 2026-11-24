// TextureCache -- see include/TextureCache.h. All given to students.

#include "TextureCache.h"

#include <cmath>
#include <cstring>

// stb_image is vendored the same way ImGui, glad and tinyfiledialogs are:
// a single public-domain header, one implementation unit. It reads PNG, JPG,
// TGA and BMP, which is what a student will actually download. Writing our own
// reader would mean telling the class "convert your texture to PPM first",
// which costs more support time than the header costs anyone.
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO_WARNING
#include "stb_image.h"

namespace {

std::map<std::string, Texture> g_textures;
std::string                    g_lastError;

} // namespace

namespace TextureCache {

const Texture* get(const std::string& path) {
    if (path.empty()) return 0;

    std::map<std::string, Texture>::iterator it = g_textures.find(path);
    if (it != g_textures.end()) {
        return it->second.valid() ? &it->second : 0;
    }

    // v = 0 at the bottom. OpenGL's texture origin is bottom-left while every
    // image format stores the top row first, so without this a texture appears
    // upside down in the viewport -- and, worse, the tracer and the viewport
    // would disagree, since the tracer samples the host copy directly.
    stbi_set_flip_vertically_on_load(1);

    int w = 0, h = 0, channels = 0;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);

    Texture& tex = g_textures[path];      // insert even on failure, so a
                                          // missing file is not re-decoded
                                          // every frame
    if (!data) {
        g_lastError = "could not load texture: " + path;
        const char* why = stbi_failure_reason();
        if (why) g_lastError += " (" + std::string(why) + ")";
        return 0;
    }

    tex.width  = w;
    tex.height = h;
    tex.pixels.assign(data, data + static_cast<size_t>(w) * h * 4);
    stbi_image_free(data);
    return &tex;
}

GLuint handleFor(const std::string& path) {
    const Texture* t = get(path);
    if (!t) return 0;

    Texture& tex = g_textures[path];
    if (tex.handle != 0) return tex.handle;

    // A headless build has no context; glGenTextures would be a null function
    // pointer through glad. Bail rather than crash -- the ray tracer and the
    // tests need the pixels, not the handle.
    if (!glGenTextures) return 0;

    glGenTextures(1, &tex.handle);
    glBindTexture(GL_TEXTURE_2D, tex.handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, tex.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex.handle;
}

void releaseAll() {
    if (glDeleteTextures) {
        for (std::map<std::string, Texture>::iterator it = g_textures.begin();
             it != g_textures.end(); ++it) {
            if (it->second.handle) glDeleteTextures(1, &it->second.handle);
        }
    }
    g_textures.clear();
}

size_t size() { return g_textures.size(); }

const std::string& lastError() { return g_lastError; }

} // namespace TextureCache
