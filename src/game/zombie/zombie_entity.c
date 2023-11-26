#include <math.h>

#include "game/zombie/zombie_entity.h"
#include "game/zombie/world.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"

void behaviour_boid(World* world, ZombieEntity* zombie) {
    const float ZOMBIE_RADIUS = 15.f;
    const float FACTOR_ZOMBIE_SEPARATION = 10.f;
    const float FACTOR_ZOMBIE_COHESION = 0.6f;
    const float FACTOR_ZOMBIE_ALIGNMENT = 0.2f;

    Vector nearby_zombies = get_nearby_zombies(world, zombie->position, zombie->vision_range);
    if (nearby_zombies.length == 0) {
        del_vector(nearby_zombies);
        return;
    }

    Vector2f close = (Vector2f){ 0.f, 0.f };
    Vector2f average_velocity = (Vector2f){ 0.f, 0.f };
    Vector2f average_position = (Vector2f){ 0.f, 0.f };
    for (int i = 0; i < nearby_zombies.length; i++) {
        ZombieEntity* near_zombie = NULL;
        near_zombie = _VECTOR_GET(ZombieEntity*, &nearby_zombies, i);

        float distance_to = distance_vector2f(zombie->position, near_zombie->position);
        if (distance_to <= ZOMBIE_RADIUS) {
            close = add_vector2f(close, sub_vector2f(zombie->position, near_zombie->position));
        }

        average_velocity = add_vector2f(average_velocity, near_zombie->velocity);
        average_position = add_vector2f(average_position, near_zombie->position);
    }

    average_velocity = mul_vector2f(average_velocity, 1.f / nearby_zombies.length);
    average_position = mul_vector2f(average_position, 1.f / nearby_zombies.length);

    Vector2f cohesion = sub_vector2f(average_position, zombie->position);
    Vector2f alignment = sub_vector2f(average_velocity, zombie->velocity);

    Vector2f acceleration = (Vector2f){ 0.f, 0.f };
    acceleration = add_vector2f(acceleration, mul_vector2f(close, FACTOR_ZOMBIE_SEPARATION));
    acceleration = add_vector2f(acceleration, mul_vector2f(cohesion, FACTOR_ZOMBIE_COHESION));
    acceleration = add_vector2f(acceleration, mul_vector2f(alignment, FACTOR_ZOMBIE_ALIGNMENT));

    zombie->acceleration = add_vector2f(acceleration, mul_vector2f(zombie->wander_direction, 50.f));

    del_vector(nearby_zombies);
}

ZombieEntity new_zombie_entity(void) {
    return (ZombieEntity) {
        .id = -1,
        .position = (Vector2f) { 0.f, 0.f },
        .acceleration = (Vector2f) { 0.f, 0.f },
        .velocity = (Vector2f) { 0.f, 0.f },
        .wander_direction = (Vector2f) { 0.f, 0.f },
        .wander_angle = 0.f,
        .vision_range = 50.f,
        .ai_state = WANDERING
    };
}

void update_zombie(World* world, ZombieEntity* zombie, float delta_time) {
    switch (zombie->ai_state) {
        case WANDERING:
            behaviour_boid(world, zombie);
            break;
        case CHASING:
            break;
    }

    Vector2f added_velocity = mul_vector2f(zombie->acceleration, delta_time);
    zombie->velocity = add_vector2f(zombie->velocity, added_velocity);

    float speed = length_vector2f(zombie->velocity);
    if (speed > 50.f) {
        zombie->velocity = mul_vector2f(normalise_vector2f(zombie->velocity), 50.f);
    }

    Vector2f added_position = mul_vector2f(zombie->velocity, delta_time);
    zombie->position = add_vector2f(zombie->position, added_position);

    float angle = frandrange(0.f, 2.f * 3.14159f);
    zombie->wander_angle += angle * delta_time;
    zombie->wander_direction.x = cos(angle);
    zombie->wander_direction.y = sin(angle);

    if (fabs(zombie->position.x) > 1280.f) {
        zombie->position.x *= -1.f;
    }

    if (fabs(zombie->position.y) > 720.f) {
        zombie->position.y *= -1.f;
    }
}
