#ifndef VESTIGE_ZOMBIE_ENTITY_H
#define VESTIGE_ZOMBIE_ENTITY_H

#include "lib/math.h"

typedef struct ZombieEntity {
    Vector2f position;
    Vector2f acceleration;
    Vector2f velocity;
    float vision_range;
    enum {
        WANDERING,
        CHASING
    } ai_state;
} ZombieEntity;

void update_zombie(ZombieEntity* zombie, float delta_time);

#endif
