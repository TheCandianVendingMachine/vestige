#ifndef VESTIGE_UI_SPRITE_H
#define VESTIGE_UI_SPRITE_H

#include "render/texture.h"

typedef struct UiSprite {
    union {
        Texture texture;
    } sprite;

    enum {
        SPRITE_NONE,
        SPRITE_TEXTURE
    } type;
} UiSprite;

#endif
