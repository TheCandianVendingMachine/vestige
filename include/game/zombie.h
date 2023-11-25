#ifndef VESTIGE_ZOMBIE_H
#define VESTIGE_ZOMBIE_H

#include "render/texture.h"
#include "render/shader.h"
#include "lib/math.h"
#include "game/zombie/world.h"

typedef struct ZombieState {
    ShaderProgram test_shader;
    World world;
    Matrix4f projection;
} ZombieState;
struct GameState;

void zombie_push(struct GameState* state);
void zombie_pop(struct GameState* state);
void zombie_update(struct GameState* state, float delta_time);
void zombie_render(struct GameState* state);

#endif
