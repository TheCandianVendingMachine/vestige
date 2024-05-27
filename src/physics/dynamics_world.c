#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_PHYSICS
#include "logger.h"

#include "physics/dynamics_world.h"
#include "physics/rigid_body.h"

DynamicsWorld create_dynamics_world(void) {
    DynamicsWorld world;
    world.rigid_bodies = new_colony(sizeof(RigidBody));
    world.gravity_acceleration = 10.f;
    world.gravity_direction = normalise_vector2f((Vector2f) { .x = 0.f, .y = 1.0f });

    return world;
}

void dynamics_world_step(DynamicsWorld* world, float delta_time) {
    Vector2f gravity = mul_vector2f(world->gravity_direction, world->gravity_acceleration);
    COLONY_ITER_BEGIN(RigidBody, world->rigid_bodies);
        if ((i->flags & SIMULATION_DISABLE) != 0) { continue; }

        i->particle = particle_simulate(i->flags, gravity, i->particle, delta_time);
    COLONY_ITER_END;
}

RigidBody* world_create_rigid_body(DynamicsWorld* world) {
    RigidBody body = create_rigid_body();
    body._id = 1 + world->rigid_bodies.length;
    size_t index = colony_insert(&world->rigid_bodies, &body);
    return colony_get(world->rigid_bodies, index);
}
