#ifndef BOTS_MISSILE_H
#define BOTS_MISSILE_H

#include "lib/math.h"
#include "lib/pid.h"

typedef struct Missile {
    Vector2f direction;
    Vector2f motor_direction;
    float dry_mass;
    struct {
        Vector2f position;
        Vector2f velocity;
        Vector2f acceleration;
    } physics;
    struct {
        float fuel_mass;
        float burn_rate;
        float thrust;
    } motor;
    struct {
        Vector2f *target;
        Vector2f last_target_position;
        float last_distance;
    } seeker;
    struct {
        Vector2f last_los;
        float gain;
    } guidance;
    struct {
        PID integral_control[2];
        PID proportional_control[2];
        Vector2f commanded_acceleration;
    } autopilot;
} Missile;

#endif
