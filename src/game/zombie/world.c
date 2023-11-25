#include "game/zombie/world.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "lib/math.h"

World create_new_world(void) {
    World world;
    world.zombies = VECTOR(ZombieEntity);

    for (int i = 0; i < 200; i++) {
        ZombieEntity zombie = new_zombie_entity();

        zombie.id = i;

        zombie.position.x = frandrange(0.f, 500.f);
        zombie.position.y = frandrange(0.f, 500.f);

        log_debug("Creating zombie #%d", i);
        VECTOR_PUSH(ZombieEntity, &world.zombies, zombie);
    }

    log_debug("New world with %d zombies", world.zombies.length);
    return world;
}

void destroy_world(World* world) {
    del_vector(world->zombies);
}

void update_world(World* world, float delta_time) {
    for (int i = 0; i < world->zombies.length; i++) {
        update_zombie(world, &_VECTOR_GET(ZombieEntity, &world->zombies, i), delta_time);
    }
}

void draw_zombies(World* world, int model_position) {
    for (int i = 0; i < world->zombies.length; i++) {
        //ZombieEntity* zombie = &_VECTOR_GET(ZombieEntity, &world->zombies, i);
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
