#ifndef TEXTURE_H
#define TEXTURE_H

#include <stddef.h>

#include "render/color.h"

typedef struct Image {
    int width;
    int height;
    int channel_count;
    size_t _pixel_size;
    union {
        ColorRGBA* rgba;
        ColorRGB* rgb;
        ColorRG* rg;
        ColorR* r;
        void* _bound;
    } pixels;
} Image;

typedef struct Texture {
    Image image;
    unsigned int id;
} Texture;

Image load_image_from_file(const char* path);
void draw_image_to_file(Image image, const char* path);
Image generate_image(int width, int height, int channels);
void set_default_image(Image* image);
void destroy_image(Image image);

Texture generate_texture(void);
void bind_image_to_texture(Texture* texture, Image image);
void destroy_texture(Texture texture);

#endif
