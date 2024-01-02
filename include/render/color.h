#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef struct ColorR {
    uint8_t r;
} ColorR;

typedef struct FColorR {
    float r;
} FColorR;

typedef union ColorRG {
    struct {
        uint8_t r;
        uint8_t g;
    };
    uint16_t rg;
} ColorRG;

typedef struct FColorRG {
    float r;
    float g;
} FColorRG;

typedef union ColorRGB {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    uint32_t rgb : 24;
} __attribute__((packed)) ColorRGB;

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

ColorR hex_to_r(const char* hex);
ColorRG hex_to_rg(const char* hex);
ColorRGB hex_to_rgb(const char* hex);
ColorRGBA hex_to_rgba(const char* hex);

FColorR color_r_to_float(ColorR color);
FColorRG color_rg_to_float(ColorRG color);
FColorRGB color_rgb_to_float(ColorRGB color);
FColorRGBA color_rgba_to_float(ColorRGBA color);

#endif
