#ifndef VESTIGE_SCENE_H
#define VESTIGE_SCENE_H

#include "game/bots/player.h"

typedef struct Scene {
    Player player;
} Scene;

void update_scene(Scene* scene, float delta_time);
void render_scene(Scene* scene);

#endif
