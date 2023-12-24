#include <string.h>

#include "glad/glad.h"
#include "render/texture.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "logger.h"

Image load_image_from_file(const char *path) {
    Image texture;

    unsigned char* data = stbi_load(path, &texture.width, &texture.height, &texture.channel_count, 0);
    texture = generate_image(texture.width, texture.height, texture.channel_count);

    memcpy(texture.pixels._bound, data, texture.channel_count * texture.width * texture.height);
    stbi_image_free(data);

    return texture;
}

void draw_image_to_file(Image image, const char* path) {
    stbi_write_bmp(path, image.width, image.height, image.channel_count, image.pixels.rgb);
}

Image generate_image(int width, int height, int channels) {
    Image image;
    image.width = width;
    image.height = height;
    image.channel_count = channels;
    switch (image.channel_count) {
        case 1:
            image._pixel_size = sizeof(ColorR) * image.width * image.height;
            image.pixels.r = malloc(image._pixel_size);
            break;
        case 2:
            image._pixel_size = sizeof(ColorRG) * image.width * image.height;
            image.pixels.rg = malloc(image._pixel_size);
            break;
        case 3:
            image._pixel_size = sizeof(ColorRGB) * image.width * image.height;
            image.pixels.rgb = malloc(image._pixel_size);
            break;
        case 4:
            image._pixel_size = sizeof(ColorRGBA) * image.width * image.height;
            image.pixels.rgba = malloc(image._pixel_size);
            break;
        default:
            log_error("Image with channel count %d not supported", image.channel_count);
            break;
    };
    set_default_image(&image);

    return image;
}

void set_default_image(Image* image) {
    const int CELL_COUNT = 4;
    const int BLOCK_SIZE_X = image->width / CELL_COUNT;
    const int BLOCK_SIZE_Y = image->height / CELL_COUNT;

    for (int i = 0; i < CELL_COUNT; i++) {
        int start_x = i * BLOCK_SIZE_X;
        for (int j = 0; j < CELL_COUNT; j++) {
            int pixel_colour = 255 * ((i % 2) == (j % 2));
            int start_y = j * BLOCK_SIZE_Y;
            for (int x = 0; x < BLOCK_SIZE_X; x++) {
                for (int y = 0; y < BLOCK_SIZE_Y; y++) {
                    int out_x = start_x + x;
                    int out_y = start_y + y;
                    switch (image->channel_count) {
                        case 1:
                            image->pixels.r[out_x + out_y * image->width].r = pixel_colour;
                            break;
                        case 2:
                            image->pixels.rg[out_x + out_y * image->width].r = 0;
                            image->pixels.rg[out_x + out_y * image->width].g = pixel_colour;
                            break;
                        case 3:
                            image->pixels.rgb[out_x + out_y * image->width].r = pixel_colour;
                            image->pixels.rgb[out_x + out_y * image->width].g = 0;
                            image->pixels.rgb[out_x + out_y * image->width].b = pixel_colour;
                            break;
                        case 4:
                            image->pixels.rgba[out_x + out_y * image->width].r = pixel_colour;
                            image->pixels.rgba[out_x + out_y * image->width].g = 0;
                            image->pixels.rgba[out_x + out_y * image->width].b = pixel_colour;
                            image->pixels.rgba[out_x + out_y * image->width].a = 255;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
}

void destroy_image(Image image) {
    free(image.pixels._bound);
}

Texture generate_texture(void) {
    Texture texture;
    glGenTextures(1, &texture.id);
    return texture;
}

void destroy_texture(Texture texture) {
    glDeleteTextures(1, &texture.id);
}

void bind_image_to_texture(Texture* texture, Image image) {
    glBindTexture(GL_TEXTURE_2D, texture->id);

    GLint format = GL_RGB;
    if (image.channel_count == 4) {
        format = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        format,
        image.width, image.height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        image.pixels._bound
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}
