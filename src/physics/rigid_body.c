#include <math.h>

#include "lib/math.h"
#include "physics/rigid_body.h"

float FRICTION_COEFFICIENT_TABLE[RIGID_BODY_MATERIAL_COUNT][RIGID_BODY_MATERIAL_COUNT] = {
    //              None | Aluminium | Ice
    /* None */      { 0.f, 0.f,        0.f,     },
    /* Aluminium */ { 0.f, 1.05f,      0.03f,   },
    /* Ice */       { 0.f, 0.03f,      0.1f,    },
};

RigidBody create_rigid_body(void) {
    return (RigidBody) {
        .flags = SIMULATION_DEFAULT,
        .collider = create_collider(BOUND_TYPE_AABB, (Bound) {
            .aabb = (ShapeAABB) {
                .position = (Vector2f) { .x = 0.f, .y = 0.f },
                .size = (Vector2f) { .x = 0.f, .y = 0.f },
            }
        }),
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .velocity = (Vector2f) { .x = 0.f, .y = 0.f },
        .acceleration = (Vector2f) { .x = 0.f, .y = 0.f },
        .linear_impulse = (Vector2f) { .x = 0.f, .y = 0.f },
        .angular_impulse = (Vector2f) { .x = 0.f, .y = 0.f },
        .friction_force = (Vector2f) { .x = 0.f, .y = 0.f },
        .restitution = 1.f,
        .material = RIGID_BODY_MATERIAL_NONE,
        .mass = 1.f,
        .rotation = 0.f,
        .transformation_matrix = new_transform()
    };
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
