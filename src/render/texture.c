#include <string.h>

#include "glad/glad.h"
#include "render/texture.h"
#include "render/stb_image.h"

Image load_image_from_file(const char *path) {
    Image texture;

    unsigned char* data = stbi_load(path, &texture.width, &texture.height, &texture.channel_count, 0);

    if (texture.channel_count == 3) {
        texture.pixels.rgb = malloc(sizeof(ColorRGB) * texture.width * texture.height);
    } else {
        texture.pixels.rgba = malloc(sizeof(ColorRGBA) * texture.width * texture.height);
    }
    memcpy(texture.pixels._bound, data, texture.channel_count * texture.width * texture.height);
    stbi_image_free(data);

    return texture;
}

void destroy_image(Image image) {
    free(image.pixels._bound);
}

Texture generate_texture(void) {
    Texture texture;
    glGenTextures(1, &texture.id);
    return texture;
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
