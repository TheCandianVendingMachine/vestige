#ifndef BOTS_BULLET_MANAGER_H
#define BOTS_BULLET_MANAGER_H

#include "lib/colony.h"
#include "lib/clock.h"
#include "lib/math.h"
#include "render/shader.h"

typedef struct BulletManager {
    Colony bullets;
    Clock _clock;
    struct {
        ShaderProgram shader;
        unsigned int vao;
    } _render;
} BulletManager;

BulletManager create_bullet_manager(void);

void bullet_manager_fixed_update(BulletManager* bullets, float delta_time);
struct GameplayState;
void bullet_manager_render(BulletManager* bullets, struct GameplayState* state);

void bullet_manager_fire(BulletManager* bullets, Vector2f origin, Vector2f direction, float speed);

#endif
