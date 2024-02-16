#ifndef VESTIGE_RIGID_BODY_H
#define VESTIGE_RIGID_BODY_H

#include "lib/math.h"
#include "transform.h"

typedef struct RigidBody {
    Vector2f dimensions;
    Vector2f position;
    Vector2f velocity;
    Vector2f acceleration;
    Vector2f linear_impulse;
    Vector2f angular_impulse;
    float mass;     // in kilograms
    float rotation; // in radians
    Transform transformation_matrix;
} RigidBody;

RigidBody create_rigid_body(void);

void rigid_body_step(RigidBody* body, float delta_time);

float rigid_body_energy(RigidBody body);
Vector2f rigid_body_linear_momentum(RigidBody body);
float rigid_body_angular_momentum(RigidBody body);

#endif
