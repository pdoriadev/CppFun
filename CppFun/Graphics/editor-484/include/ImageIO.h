#ifndef IMAGEIO_H
#define IMAGEIO_H

#include <string>
#include <vector>

// Minimal image WRITERS. Reading is TextureCache's job, which does use stb --
// see the note there on why a hand-written reader was not worth the student
// support cost. These stay dependency-free because writing is easy and the
// tracer's output path should not depend on a vendored header.
//
//   .ppm - P6 binary, the canonical format in graphics coursework
//   .bmp - 24-bit uncompressed, opens by double-click on Windows
namespace ImageIO {

// `pixels` is tightly packed RGB8, row 0 = top of image.
bool writePPM(const std::string& path, const std::vector<unsigned char>& pixels,
              int width, int height);

bool writeBMP(const std::string& path, const std::vector<unsigned char>& pixels,
              int width, int height);

// Dispatches on file extension; defaults to PPM if unrecognized.
bool write(const std::string& path, const std::vector<unsigned char>& pixels,
           int width, int height);

} // namespace ImageIO

#endif // IMAGEIO_H
