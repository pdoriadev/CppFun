#include "ImageIO.h"

#include <fstream>
#include <algorithm>
#include <cctype>

namespace {

std::string lowerExtension(const std::string& path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) return "";
    std::string ext = path.substr(dot + 1);
    for (size_t i = 0; i < ext.size(); ++i) {
        ext[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(ext[i])));
    }
    return ext;
}

void put16(std::ofstream& out, unsigned short v) {
    unsigned char b[2] = { static_cast<unsigned char>(v & 0xFF),
                           static_cast<unsigned char>((v >> 8) & 0xFF) };
    out.write(reinterpret_cast<char*>(b), 2);
}

void put32(std::ofstream& out, unsigned int v) {
    unsigned char b[4] = { static_cast<unsigned char>(v & 0xFF),
                           static_cast<unsigned char>((v >> 8) & 0xFF),
                           static_cast<unsigned char>((v >> 16) & 0xFF),
                           static_cast<unsigned char>((v >> 24) & 0xFF) };
    out.write(reinterpret_cast<char*>(b), 4);
}

} // namespace

namespace ImageIO {

bool writePPM(const std::string& path, const std::vector<unsigned char>& pixels,
              int width, int height) {
    if (width <= 0 || height <= 0) return false;
    if (pixels.size() < static_cast<size_t>(width) * height * 3) return false;

    std::ofstream out(path.c_str(), std::ios::binary);
    if (!out) return false;

    out << "P6\n" << width << " " << height << "\n255\n";
    out.write(reinterpret_cast<const char*>(&pixels[0]),
              static_cast<std::streamsize>(width) * height * 3);
    return out.good();
}

bool writeBMP(const std::string& path, const std::vector<unsigned char>& pixels,
              int width, int height) {
    if (width <= 0 || height <= 0) return false;
    if (pixels.size() < static_cast<size_t>(width) * height * 3) return false;

    std::ofstream out(path.c_str(), std::ios::binary);
    if (!out) return false;

    // BMP rows are padded to 4-byte boundaries.
    const int rowBytes = width * 3;
    const int padding  = (4 - (rowBytes % 4)) % 4;
    const unsigned int imageSize = static_cast<unsigned int>((rowBytes + padding) * height);
    const unsigned int fileSize  = 14 + 40 + imageSize;

    out.put('B'); out.put('M');
    put32(out, fileSize);
    put16(out, 0);
    put16(out, 0);
    put32(out, 14 + 40);          // pixel data offset

    put32(out, 40);               // DIB header size
    put32(out, static_cast<unsigned int>(width));
    put32(out, static_cast<unsigned int>(height));
    put16(out, 1);                // planes
    put16(out, 24);               // bits per pixel
    put32(out, 0);                // BI_RGB, no compression
    put32(out, imageSize);
    put32(out, 2835);             // ~72 DPI
    put32(out, 2835);
    put32(out, 0);
    put32(out, 0);

    const unsigned char pad[3] = { 0, 0, 0 };

    // BMP stores rows bottom-up, and channels as BGR.
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            size_t i = (static_cast<size_t>(y) * width + x) * 3;
            unsigned char bgr[3] = { pixels[i + 2], pixels[i + 1], pixels[i] };
            out.write(reinterpret_cast<char*>(bgr), 3);
        }
        if (padding) out.write(reinterpret_cast<const char*>(pad), padding);
    }

    return out.good();
}

bool write(const std::string& path, const std::vector<unsigned char>& pixels,
           int width, int height) {
    std::string ext = lowerExtension(path);
    if (ext == "bmp") return writeBMP(path, pixels, width, height);
    return writePPM(path, pixels, width, height);
}

} // namespace ImageIO
