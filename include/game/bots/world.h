#ifndef BOTS_WORLD_H
#define BOTS_WORLD_H

#include "game/bots/resource_map.h"

typedef struct World {
    ResourceMap resources;
} World;

struct FontEngine;
struct GameplayState;

World new_world(struct FontEngine* font_engine);
void render_world(struct GameplayState* state, World* world);

#endif
