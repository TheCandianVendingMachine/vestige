#include <stdlib.h>

#include "render/color.h"
#include "logger.h"

ColorR hex_to_r(const char* hex) {
    ColorR color;

    unsigned long hex_as_int = strtol(hex, NULL, 16);

    color.r = hex_as_int & 0xFF;

    return color;
}

ColorRG hex_to_rg(const char* hex) {
    ColorRG color;

    unsigned long hex_as_int = strtol(hex, NULL, 16);

    color.r = (hex_as_int & 0xFF00) >> 8;
    color.g = (hex_as_int & 0x00FF) >> 0;

    return color;
}

ColorRGB hex_to_rgb(const char* hex) {
    ColorRGB color;

    unsigned long hex_as_int = strtol(hex, NULL, 16);

    color.r = (hex_as_int & 0xFF0000) >> 16;
    color.g = (hex_as_int & 0x00FF00) >> 8;
    color.b = (hex_as_int & 0x0000FF) >> 0;

    return color;
}

ColorRGBA hex_to_rgba(const char* hex) {
    ColorRGBA color;

    unsigned long hex_as_int = strtol(hex, NULL, 16);

    color.r = (hex_as_int & 0xFF000000) >> 24;
    color.g = (hex_as_int & 0x00FF0000) >> 16;
    color.b = (hex_as_int & 0x0000FF00) >> 8;
    color.a = (hex_as_int & 0x000000FF) >> 0;

    return color;
}

FColorR color_r_to_float(ColorR* color) {
    FColorR f_color;

    f_color.r = (float)color->r / 255.f;

    return f_color;
}

FColorRG color_rg_to_float(ColorRG* color) {
    FColorRG f_color;

    f_color.r = (float)color->r / 255.f;
    f_color.g = (float)color->g / 255.f;

    return f_color;
}

FColorRGB color_rgb_to_float(ColorRGB* color) {
    FColorRGB f_color;

    f_color.r = (float)color->r / 255.f;
    f_color.g = (float)color->g / 255.f;
    f_color.b = (float)color->b / 255.f;

    return f_color;
}

FColorRGBA color_rgba_to_float(ColorRGBA* color) {
    FColorRGBA f_color;

    f_color.r = (float)color->r / 255.f;
    f_color.g = (float)color->g / 255.f;
    f_color.b = (float)color->b / 255.f;
    f_color.a = (float)color->a / 255.f;

    return f_color;
}
