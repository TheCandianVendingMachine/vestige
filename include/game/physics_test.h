#ifndef GAME_PHYSICS_H
#define GAME_PHYSICS_H

#include "lib/math.h"
#include "render/camera.h"
#include "debug/render.h"

typedef struct PhysicsTestState {
    DebugRender renderer;
    Camera camera;
    Matrix4f projection;
} PhysicsTestState ;
struct GameState;

void physics_push(struct GameState* state);
void physics_pop(struct GameState* state);
void physics_update(struct GameState* state);
void physics_fixed_update(struct GameState* state, float delta_time);
void physics_render(struct GameState* state);

#endif