#ifndef VESTIGE_ZOMBIE_ENTITY_H
#define VESTIGE_ZOMBIE_ENTITY_H

#include "lib/math.h"

typedef struct ZombieEntity {
    int id;
    Vector2f position;
    Vector2f acceleration;
    Vector2f velocity;
    Vector2f wander_direction;
    float wander_angle;
    float vision_range;
    enum {
        WANDERING,
        CHASING
    } ai_state;
} ZombieEntity;

ZombieEntity new_zombie_entity(void);

struct World;
void update_zombie(struct World* world, ZombieEntity* zombie, float delta_time);

#endif
