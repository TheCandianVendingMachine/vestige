#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_PHYSICS
#include "logger.h"
#include "debug/render.h"

#include "physics/dynamics_world.h"
#include "physics/rigid_body.h"

void dynamics_world_debug_draw(DynamicsWorld* world, DebugRender* debug_renderer) {
    const float c_max_length = 100.f;
    DebugShapeLine force_line;
    force_line.thickness = 0.02f;

    COLONY_ITER_BEGIN(RigidBody, world->last_tick_rigid_bodies);
        if ((i->flags & SIMULATION_DISABLE) != 0) { continue; }
        if (true || (i->flags & SIMULATION_DEBUG_DRAW) != 0) {
            force_line.position = i->particle.position;

            float max_magnitude = length2_vector2f(i->particle.impulse);
            max_magnitude = max_float(max_magnitude, length2_vector2f(i->particle.normal_force));
            max_magnitude = max_float(max_magnitude, length2_vector2f(i->particle.friction_force));

            if (max_magnitude == 0.f) { continue; }

            // Total Force
            force_line.colour = hex_to_rgb("0xFFFFFF");
            if (length2_vector2f(i->particle.impulse) != 0.f) {
                force_line.direction = normalise_vector2f(i->particle.impulse);
                force_line.distance = c_max_length * length2_vector2f(i->particle.impulse) / max_magnitude;
                debug_line(debug_renderer, force_line);
            }

            // Normal Force
            force_line.colour = hex_to_rgb("0xAA0099");
            if (length2_vector2f(i->particle.normal_force) != 0.f) {
                force_line.direction = normalise_vector2f(i->particle.normal_force);
                force_line.distance = c_max_length * length2_vector2f(i->particle.normal_force) / max_magnitude;
                debug_line(debug_renderer, force_line);
            }

            // Friction Force
            force_line.colour = hex_to_rgb("0x00AA22");
            if (length2_vector2f(i->particle.friction_force) != 0.f) {
                force_line.direction = normalise_vector2f(i->particle.friction_force);
                force_line.distance = c_max_length * length2_vector2f(i->particle.friction_force) / max_magnitude;
                debug_line(debug_renderer, force_line);
            }
        }
    COLONY_ITER_END;
}

DynamicsWorld create_dynamics_world(void) {
    DynamicsWorld world;
    world.rigid_bodies = new_colony(sizeof(RigidBody));
    world.last_tick_rigid_bodies = new_colony(sizeof(RigidBody));
    world.gravity_acceleration = 10.f;
    world.gravity_direction = normalise_vector2f((Vector2f) { .x = 0.f, .y = 1.0f });

    return world;
}

void dynamics_world_pre_step(DynamicsWorld* world) {
    colony_clear(&world->last_tick_rigid_bodies);
    COLONY_ITER_BEGIN(RigidBody, world->rigid_bodies);
        if ((i->flags & SIMULATION_DISABLE) != 0) { continue; }
        colony_insert(&world->last_tick_rigid_bodies, i);
    COLONY_ITER_END;
}

void dynamics_world_step(DynamicsWorld* world, float delta_time) {
    Vector2f gravity = mul_vector2f(world->gravity_direction, world->gravity_acceleration);
    COLONY_ITER_BEGIN(RigidBody, world->rigid_bodies);
        if ((i->flags & SIMULATION_DISABLE) != 0) { continue; }

        colony_insert(&world->last_tick_rigid_bodies, i);
        i->particle = particle_simulate(i->flags, gravity, i->particle, delta_time);
    COLONY_ITER_END;
}

RigidBody* world_create_rigid_body(DynamicsWorld* world) {
    RigidBody body = create_rigid_body();
    body._id = 1 + world->rigid_bodies.length;
    size_t index = colony_insert(&world->rigid_bodies, &body);
    return colony_get(world->rigid_bodies, index);
}
