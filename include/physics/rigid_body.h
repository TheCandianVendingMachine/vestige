#ifndef VESTIGE_RIGID_BODY_H
#define VESTIGE_RIGID_BODY_H

#include "physics/collider.h"
#include "physics/simulation_flags.h"
#include "physics/particle.h"
#include "lib/math.h"
#include "transform.h"

typedef struct RigidBody {
    uint64_t _id;
    SimulationFlags flags;
    Particle particle;
    Collider collider;
    Vector2f angular_impulse;
    float rotation; // in radians
    Transform transformation_matrix;
} RigidBody;


RigidBody create_rigid_body(void);
float rigid_body_angular_momentum(RigidBody body);

#endif
