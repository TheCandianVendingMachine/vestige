#include <math.h>

#include "lib/math.h"
#include "physics/rigid_body.h"

RigidBody create_rigid_body(void) {
    return (RigidBody) {
        .flags = SIMULATION_DEFAULT,
        .collider = OPTION_NONE(Collider),
        .angular_impulse = (Vector2f) { .x = 0.f, .y = 0.f },
        .rotation = 0.f,
        .transformation_matrix = new_transform()
    };
}

float rigid_body_angular_momentum(RigidBody body) {
    return 0.f;
}
