#include "physics/particle.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_PHYSICS
#include "logger.h"

float FRICTION_COEFFICIENT_TABLE[PARTICLE_MATERIAL_COUNT][PARTICLE_MATERIAL_COUNT] = {
    //              None | Aluminium | Ice
    /* None */      { 0.f, 0.f,        0.f,     },
    /* Aluminium */ { 0.f, 1.05f,      0.03f,   },
    /* Ice */       { 0.f, 0.03f,      0.1f,    },
};

Particle create_particle(void) {
    Particle p;
    p.mass = 1.f;
    p.material = PARTICLE_MATERIAL_NONE;
    p.position = (Vector2f) { .x = 0.f, .y = 0.f };
    p.velocity = (Vector2f) { .x = 0.f, .y = 0.f };
    p.acceleration = (Vector2f) { .x = 0.f, .y = 0.f };
    p.friction_force = (Vector2f) { .x = 0.f, .y = 0.f };
    p.normal_force = (Vector2f) { .x = 0.f, .y = 0.f };
    p.impulse = (Vector2f) { .x = 0.f, .y = 0.f };
    p.restitution = 1.f;
    return p;
}

float particle_energy(Particle particle) {
    return 0.5f * particle.mass * dot_vector2f(particle.velocity, particle.velocity);
}

Vector2f particle_momentum(Particle particle) {
    return mul_vector2f(particle.velocity, particle.mass);
}

Particle particle_simulate(SimulationFlags flags, Vector2f gravity, Particle p, float delta_time) {
    if ((flags & SIMULATION_DISABLE_GRAVITY) == 0) {
        p.acceleration = add_vector2f(p.acceleration, gravity);
    }

    Vector2f force = p.impulse;
    force = add_vector2f(force, p.normal_force);

    // friction
    {
        p.friction_force = (Vector2f) { .x = 0.f, .y = 0.f };

        float sign_vx = fsign(p.velocity.x);
        float sign_vy = fsign(p.velocity.y);

        p.friction_force.x = -sign_vx * p.friction_force.y;
        p.friction_force.y = sign_vy * p.friction_force.x;

        if (length2_vector2f(force) != 0.f) {
            p.friction_force = project_vector2f(p.friction_force, force);
        }
        force = add_vector2f(force, p.friction_force);
    }

    p.normal_force = (Vector2f) { .x = 0.f, .y = 0.f };
    p.impulse = (Vector2f) { .x = 0.f, .y = 0.f };

    p.acceleration = add_vector2f(p.acceleration, mul_vector2f(force, 1.f / p.mass));
    p.velocity = add_vector2f(p.velocity, mul_vector2f(p.acceleration, delta_time));
    p.position = add_vector2f(p.position, mul_vector2f(p.velocity, delta_time));

    return p;
}

void particle_debug_report(Particle particle) {
    log_debug_lines("Particle [mass: %f, restitution: %f, material: %d]", particle.mass, particle.restitution, particle.material);
    log_line("Position: %f %f", particle.position.x, particle.position.y);
    log_line("Velocity: %f %f", particle.velocity.x, particle.velocity.y);
    log_line("Acceleration: %f %f", particle.acceleration.x, particle.acceleration.y);
    log_line("Acceleration: %f %f", particle.restitution, particle.acceleration.y);
    log_line_commit();
}
