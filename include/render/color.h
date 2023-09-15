#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef union ColorRGB {
    struct {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    };
    uint32_t RGB;
} ColorRGB;

typedef struct FColorRGB {
    float R;
    float G;
    float B;
} FColorRGB;

typedef union ColorRGBA {
    struct {
        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;
    };
    uint32_t RGBA;
} ColorRGBA;

typedef struct FColorRGBA {
    float R;
    float G;
    float B;
    float A;
} FColorRGBA;

ColorRGB hex_to_rgb(const char *hex);
ColorRGBA hex_to_rgba(const char *hex);

FColorRGB color_rgb_to_float(ColorRGB *color);
FColorRGBA color_rgba_to_float(ColorRGBA *color);

#endif
