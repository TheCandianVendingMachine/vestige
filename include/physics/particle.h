#ifndef VESTIGE_PARTICLE_H
#define VESTIGE_PARTICLE_H

#include "lib/math.h"
#include "physics/simulation_flags.h"

typedef enum ParticleMaterial {
    PARTICLE_MATERIAL_NONE,
    PARTICLE_MATERIAL_ALUMINIUM,
    PARTICLE_MATERIAL_ICE,
    PARTICLE_MATERIAL_COUNT
} ParticleMaterial;

extern float FRICTION_COEFFICIENT_TABLE[PARTICLE_MATERIAL_COUNT][PARTICLE_MATERIAL_COUNT];

typedef struct Particle {
    Vector2f position;
    Vector2f velocity;
    Vector2f acceleration;
    Vector2f friction_force;
    Vector2f normal_force;
    Vector2f impulse;
    float restitution; // How bouncy
    float mass;     // in kilograms
    ParticleMaterial material;
} Particle;

Particle create_particle(void);
float particle_energy(Particle particle);
Vector2f particle_momentum(Particle particle);

Particle particle_simulate(SimulationFlags flags, Vector2f gravity, Particle particle, float delta_time);

#endif
