#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef union ColorRGB {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    uint32_t rgb;
} ColorRGB;

typedef struct FColorRGB {
    float r;
    float g;
    float b;
} FColorRGB;

typedef union ColorRGBA {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    uint32_t rgba;
} ColorRGBA;

typedef struct FColorRGBA {
    float r;
    float g;
    float b;
    float a;
} FColorRGBA;

ColorRGB hex_to_rgb(const char *hex);
ColorRGBA hex_to_rgba(const char *hex);

FColorRGB color_rgb_to_float(ColorRGB *color);
FColorRGBA color_rgba_to_float(ColorRGBA *color);

#endif
