#ifndef TEXTURE_H
#define TEXTURE_H

#include "render/color.h"

typedef struct Image {
    int width;
    int height;
    int channel_count;
    union {
        ColorRGBA* rgba;
        ColorRGB* rgb;
        void* _bound;
    } pixels;
} Image;

typedef struct Texture {
    Image image;
    unsigned int id;
} Texture;

Image load_image_from_file(const char* path);
void destroy_image(Image image);

Texture generate_texture(void);
void bind_image_to_texture(Texture* texture, Image image);

#endif
