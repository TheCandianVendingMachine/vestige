#ifndef VESTIGE_PLAYER_H
#define VESTIGE_PLAYER_H

#include "render/texture.h"
#include "transform.h"

typedef struct Player {
    Transform transform;
    Texture sprite;
} Player;

#endif
