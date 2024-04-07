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
        if ((i->flags & SIMULATION_DISABLE_GRAVITY) == 0) {
            i->acceleration = add_vector2f(i->acceleration, gravity);
        }

        Vector2f force = i->linear_impulse;
        force = add_vector2f(force, i->normal_force);

        if (length2_vector2f(i->friction_force) > 0.f) {
            float sign_vx = fsign(i->velocity.x);
            float sign_vy = fsign(i->velocity.y);

            i->friction_force.x = -sign_vx * i->friction_force.y;
            i->friction_force.y = sign_vy * i->friction_force.x;

            i->friction_force = project_vector2f(i->friction_force, force);
            force = add_vector2f(force, i->friction_force);
        }

        i->friction_force = (Vector2f) { .x = 0.f, .y = 0.f };
        i->normal_force = (Vector2f) { .x = 0.f, .y = 0.f };
        i->linear_impulse = (Vector2f) { .x = 0.f, .y = 0.f };

        i->acceleration = add_vector2f(i->acceleration, mul_vector2f(force, 1.f / i->mass));
        i->velocity = add_vector2f(i->velocity, mul_vector2f(i->acceleration, delta_time));
        i->position = add_vector2f(i->position, mul_vector2f(i->velocity, delta_time));
    COLONY_ITER_END;
}

RigidBody* world_create_rigid_body(DynamicsWorld* world) {
    RigidBody body = create_rigid_body();
    body._id = 1 + world->rigid_bodies.length;
    size_t index = colony_insert(&world->rigid_bodies, &body);
    return colony_get(world->rigid_bodies, index);
}
