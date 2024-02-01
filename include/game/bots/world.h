#ifndef BOTS_WORLD_H
#define BOTS_WORLD_H

#include "render/shader.h"
#include "game/bots/resource_map.h"
#include "game/bots/bullet_manager.h"
#include "game/bots/missile_manager.h"
#include "lib/colony.h"

typedef struct World {
    ResourceMap resources;
    MissileManager missile_manager;
    BulletManager bullet_manager;
    Colony bots;
    struct {
        ShaderProgram shader;
        unsigned int vao;
    } bot_render_data;
    Clock fire_time;
    int fired;
} World;

struct FontEngine;
struct GameplayState;

World new_world(struct FontEngine* font_engine);
void update_world(World* world);
void update_world_fixed(World* world, float delta_time);
void render_world(struct GameplayState* state, World* world);

#endif
