#include <glad/glad.h>

#include "game/zombie/world.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "lib/math.h"

#include "render/vertex.h"
#include "render/primitives.h"

World create_new_world(void) {
    World world;
    world.zombies = VECTOR(ZombieEntity);

    for (int i = 0; i < 1000; i++) {
        ZombieEntity zombie = new_zombie_entity();

        zombie.id = i;

        zombie.position.x = frandrange(-1280.f, 1280.f);
        zombie.position.y = frandrange(-720.f, 720.f);

        zombie.velocity.x = frandrange(-50.f, 50.f);
        zombie.velocity.y = frandrange(-50.f, 50.f);

        zombie.wander_angle = frandrange(0.f, 2.f * 3.14159f);

        log_debug("Creating zombie #%d", i);
        VECTOR_PUSH(ZombieEntity, &world.zombies, zombie);
    }

    glGenVertexArrays(1, &world.zombie_vao);
    bind_primitive_to_vao(primitive_quad(), world.zombie_vao);

    log_debug("New world with %d zombies", world.zombies.length);
    return world;
}

void destroy_world(World* world) {
    del_vector(world->zombies);
    glDeleteVertexArrays(1, &world->zombie_vao);
}

void update_world(World* world, float delta_time) {
    for (int i = 0; i < world->zombies.length; i++) {
        update_zombie(world, &_VECTOR_GET(ZombieEntity, &world->zombies, i), delta_time);
    }
}

void draw_zombies(World* world, int model_position) {
    for (int i = 0; i < world->zombies.length; i++) {
        ZombieEntity* zombie = &_VECTOR_GET(ZombieEntity, &world->zombies, i);
        Matrix4f zombie_model = (Matrix4f) {
            {
                10.f,               0.f,                0.f, 0.f,
                0.f,                10.f,               0.f, 0.f,
                0.f,                0.f,                1.f, 0.f,
                zombie->position.x, zombie->position.y, 0.f, 1.f
            }
        };
        glUniformMatrix4fv(model_position, 1, false, zombie_model.entries);
        draw_primitive(primitive_quad(), world->zombie_vao);
    }
}

Vector get_nearby_zombies(World* world, Vector2f origin, float radius) {
    Vector zombie_vector = VECTOR(ZombieEntity*);

    for (int i = 0; i < world->zombies.length; i++) {
        ZombieEntity* zombie = &_VECTOR_GET(ZombieEntity, &world->zombies, i);

        if (distance_vector2f(origin, zombie->position) <= radius) {
            VECTOR_PUSH(ZombieEntity*, &zombie_vector, zombie);
        }
    }

    return zombie_vector;
}