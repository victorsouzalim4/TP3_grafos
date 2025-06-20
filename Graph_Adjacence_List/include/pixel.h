#ifndef PIXEL_H
#define PIXEL_H

#include <string>
#include <functional>
#include <cmath>
#include <cstdint>

struct Pixel {
    int x, y;
    uint8_t intensity;

    Pixel() : x(0), y(0), intensity(0) {}
    Pixel(int x, int y, uint8_t intensity) : x(x), y(y), intensity(intensity) {}

    bool operator==(const Pixel& other) const;
    bool operator!=(const Pixel& other) const;
    bool operator<(const Pixel& other) const;

    double distanceTo(const Pixel& other) const;
    int toLinearIndex(int imageWidth) const;
    static Pixel fromLinearIndex(int linearIndex, int imageWidth);

    std::string toString() const;
};

struct PixelHash {
    std::size_t operator()(const Pixel& p) const;
};

#endif