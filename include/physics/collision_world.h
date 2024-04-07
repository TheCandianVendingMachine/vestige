#ifndef VESTIGE_COLLISION_WORLD_H
#define VESTIGE_COLLISION_WORLD_H

#include "lib/colony.h"

typedef struct CollisionWorld {
    Colony colliders;
} CollisionWorld;

CollisionWorld create_collision_world(void);

#endif
