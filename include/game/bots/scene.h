#ifndef BOTS_SCENE_H
#define BOTS_SCENE_H

#include "game/bots/world.h"

typedef struct Scene {
    World world;
} Scene;

void update_scene(Scene* scene, float delta_time);
void render_scene(Scene* scene);

#endif
