#include "physics/rigid_body.h"

RigidBody create_rigid_body(void) {
    return (RigidBody) {
        .bounds = (AABB) { 
            .position = (Vector2f) { .x = 0.f, .y = 0.f },
            .size = { .x = 0.f, .y = 0.f },
        },
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .velocity = (Vector2f) { .x = 0.f, .y = 0.f },
        .acceleration = (Vector2f) { .x = 0.f, .y = 0.f },
        .linear_impulse = (Vector2f) { .x = 0.f, .y = 0.f },
        .angular_impulse = (Vector2f) { .x = 0.f, .y = 0.f },
        .restitution = 1.f,
        .mass = 1.f,
        .rotation = 0.f,
        .transformation_matrix = new_transform()
    };
}

void rigid_body_step(RigidBody* body, float delta_time) {
    Vector2f force = mul_vector2f(body->linear_impulse, 1.f / delta_time);
    body->linear_impulse = (Vector2f) { .x = 0.f, .y = 0.f };

    body->acceleration = add_vector2f(body->acceleration, mul_vector2f(force, 1.f / body->mass));
    body->velocity = add_vector2f(body->velocity, mul_vector2f(body->acceleration, delta_time));
    body->position = add_vector2f(body->position, mul_vector2f(body->velocity, delta_time));

    transform_set_position(&body->transformation_matrix, (Vector3f) { .x = body->position.x, .y = body->position.y, .z = 0.f });
}

float rigid_body_energy(RigidBody body) {
    return 0.5f * body.mass * dot_vector2f(body.velocity, body.velocity);
}

Vector2f rigid_body_linear_momentum(RigidBody body) {
    return mul_vector2f(body.velocity, body.mass);
}

float rigid_body_angular_momentum(RigidBody body) {
    return 0.f;
}
