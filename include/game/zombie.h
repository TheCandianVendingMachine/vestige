#ifndef VESTIGE_ZOMBIE_H
#define VESTIGE_ZOMBIE_H

#include "render/texture.h"
#include "render/shader.h"
#include "lib/math.h"

typedef struct ZombieState {
    Texture test_texture;
    ShaderProgram test_shader;
    unsigned int vao;
    Matrix4f projection;
} ZombieState;
struct GameState;

void zombie_push(struct GameState* state);
void zombie_pop(struct GameState* state);
void zombie_update(struct GameState* state, float delta_time);
void zombie_render(struct GameState* state);

#endif
