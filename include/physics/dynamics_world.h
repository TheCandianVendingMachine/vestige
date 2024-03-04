#ifndef VESTIGE_DYNAMICS_WORLD_H
#define VESTIGE_DYNAMICS_WORLD_H

#include "lib/colony.h"
#include "lib/math.h"

typedef struct DynamicsWorld {
    Colony rigid_bodies;
    float gravity_acceleration;
    Vector2f gravity_direction;
} DynamicsWorld;

DynamicsWorld create_dynamics_world(void);

void dynamics_world_step(DynamicsWorld* world, float delta_time);

struct RigidBody;
struct RigidBody* world_create_rigid_body(DynamicsWorld* world);

#endif
