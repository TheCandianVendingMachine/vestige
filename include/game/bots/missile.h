#ifndef BOTS_MISSILE_H
#define BOTS_MISSILE_H

#include "lib/math.h"

typedef struct Missile {
    Vector2f direction;
    Vector2f position;
    Vector2f velocity;
    Vector2f motor_direction;
    float dry_mass;
    struct {
        float fuel_mass;
        float burn_rate;
        float thrust;
    } motor;
    struct {
        Vector2f *target;
        float last_distance;
    } seeker;
    struct {
        Vector2f last_los;
        float gain;
    } guidance;
} Missile;

#endif
