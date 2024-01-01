#ifndef BOTS_SCENE_H
#define BOTS_SCENE_H

#include "game/bots/world.h"
#include "render/camera.h"

typedef struct Scene {
    World world;
    Camera camera;
} Scene;

struct GameplayState;

void update_scene(Scene* scene, float delta_time);
void render_scene(struct GameplayState* state, Scene* scene);

#endif
