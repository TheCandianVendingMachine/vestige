#ifndef VESTIGE_WORLD_H
#define VESTIGE_WORLD_H

#include "lib/vector.h"
#include "lib/math.h"
#include "game/zombie/zombie_entity.h"

typedef struct World {
    Vector zombies;
} World;

World create_new_world(void);
void destroy_world(World* world);

void update_world(World* world, float delta_time);
void draw_zombies(World* world, int model_position);

Vector get_nearby_zombies(World* world, Vector2f origin, float radius);

#endif
