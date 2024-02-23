#ifndef VESTIGE_RIGID_BODY_H
#define VESTIGE_RIGID_BODY_H

#include "physics/collider.h"
#include "lib/math.h"
#include "transform.h"

typedef enum RigidBodyMaterial {
    RIGID_BODY_MATERIAL_NONE,
    RIGID_BODY_MATERIAL_ALUMINIUM,
    RIGID_BODY_MATERIAL_ICE,
    RIGID_BODY_MATERIAL_COUNT
} RigidBodyMaterial;

typedef struct RigidBody {
    Collider collider;
    Vector2f position;
    Vector2f velocity;
    Vector2f acceleration;
    Vector2f linear_impulse;
    Vector2f angular_impulse;
    Vector2f normal_force;
    float restitution;
    RigidBodyMaterial material;
    float mass;     // in kilograms
    float rotation; // in radians
    Transform transformation_matrix;
} RigidBody;

extern float FRICTION_COEFFICIENT_TABLE[RIGID_BODY_MATERIAL_COUNT][RIGID_BODY_MATERIAL_COUNT];

RigidBody create_rigid_body(void);

void rigid_body_step(RigidBody* body, float delta_time);

float rigid_body_energy(RigidBody body);
Vector2f rigid_body_linear_momentum(RigidBody body);
float rigid_body_angular_momentum(RigidBody body);

#endif
