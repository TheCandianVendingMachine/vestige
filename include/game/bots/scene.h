#ifndef BOTS_SCENE_H
#define BOTS_SCENE_H

#include "game/bots/world.h"
#include "render/camera.h"
#include "game/bots/camera_controller.h"

typedef struct Scene {
    World world;
    Camera* camera;
    CameraController* camera_controller;
} Scene;

struct GameplayState;

Scene create_scene(struct GameplayState* state);
void update_scene(Scene* scene);
void update_scene_fixed(Scene* scene, float delta_time);
void render_scene(struct GameplayState* state, Scene* scene);

#endif
