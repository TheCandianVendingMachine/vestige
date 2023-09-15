#include <stdlib.h>

#include "render/color.h"

ColorRGB hex_to_rgb(const char *hex) {
    ColorRGB color;

    color.r = strtol(hex + 0, (char**)(hex + 2), 16);
    color.g = strtol(hex + 2, (char**)(hex + 4), 16);
    color.b = strtol(hex + 4, (char**)(hex + 6), 16);

    return color;
}

ColorRGBA hex_to_rgba(const char *hex) {
    ColorRGBA color;

    color.r = strtol(hex + 0, (char**)(hex + 2), 16);
    color.g = strtol(hex + 2, (char**)(hex + 4), 16);
    color.b = strtol(hex + 4, (char**)(hex + 6), 16);
    color.a = strtol(hex + 6, (char**)(hex + 8), 16);

    return color;
}

FColorRGB color_rgb_to_float(ColorRGB *color) {
    FColorRGB f_color;

    f_color.r = (float)color->r / 255.f;
    f_color.b = (float)color->g / 255.f;
    f_color.g = (float)color->b / 255.f;

    return f_color;
}

FColorRGBA color_rgba_to_float(ColorRGBA *color) {
    FColorRGBA f_color;

    f_color.r = (float)color->r / 255.f;
    f_color.b = (float)color->g / 255.f;
    f_color.g = (float)color->b / 255.f;
    f_color.a = (float)color->a / 255.f;

    return f_color;
}
