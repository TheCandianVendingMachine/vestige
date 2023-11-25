#ifndef VESTIGE_WORLD_H
#define VESTIGE_WORLD_H

#include "lib/vector.h"
#include "lib/math.h"
#include "game/zombie/zombie_entity.h"

typedef struct World {
    int _;
} World;

Vector get_nearby_zombies(Vector2f origin, float radius);

#endif
