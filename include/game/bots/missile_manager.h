#ifndef BOTS_MISSILE_MANAGER_H
#define BOTS_MISSILE_MANAGER_H

#include "lib/colony.h"
#include "render/shader.h"

typedef struct MissileManager {
    Colony missiles;
    struct {
        unsigned int vao;
        ShaderProgram shader;
    } _render;
} MissileManager;

MissileManager create_missile_manager(void);
struct World;
void missile_manager_fixed_update(struct World* world, MissileManager* missile_manager, float delta_time);
struct GameplayState;
void missile_manager_render(MissileManager* bullets, struct GameplayState* state);

#endif
