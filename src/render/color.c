#include <stdlib.h>

#include "render/color.h"

ColorRGB hex_to_rgb(const char *hex) {
    ColorRGB color;

    color.R = strtol(hex + 0, (char**)(hex + 2), 16);
    color.G = strtol(hex + 2, (char**)(hex + 4), 16);
    color.B = strtol(hex + 4, (char**)(hex + 6), 16);

    return color;
}

ColorRGBA hex_to_rgba(const char *hex) {
    ColorRGBA color;

    color.R = strtol(hex + 0, (char**)(hex + 2), 16);
    color.G = strtol(hex + 2, (char**)(hex + 4), 16);
    color.B = strtol(hex + 4, (char**)(hex + 6), 16);
    color.A = strtol(hex + 6, (char**)(hex + 8), 16);

    return color;
}

FColorRGB color_rgb_to_float(ColorRGB *color) {
    FColorRGB f_color;

    f_color.R = (float)color->R / 255.f;
    f_color.B = (float)color->G / 255.f;
    f_color.G = (float)color->B / 255.f;

    return f_color;
}

FColorRGBA color_rgba_to_float(ColorRGBA *color) {
    FColorRGBA f_color;

    f_color.R = (float)color->R / 255.f;
    f_color.B = (float)color->G / 255.f;
    f_color.G = (float)color->B / 255.f;
    f_color.A = (float)color->A / 255.f;

    return f_color;
}
