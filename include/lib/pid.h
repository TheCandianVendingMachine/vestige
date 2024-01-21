#ifndef VESTIGE_PID_H
#define VESTIGE_PID_H

#include "lib/clock.h"

typedef struct PID {
    float gain_proportional;
    float gain_derivative;
    float gain_integral;
    float _integral;
    float _last_error;
} PID;

PID new_pid(float kp, float kd, float ki);
float pid_step(PID* pid, float error, float dt);

#endif
