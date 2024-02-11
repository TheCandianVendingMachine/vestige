#include <math.h>

#include "lib/pid.h"

PID new_pid(float kp, float kd, float ki) {
    return (PID) {
        .gain_proportional = kp,
        .gain_derivative = kd,
        .gain_integral = ki,
        ._integral = 0.f,
        ._last_error = 0.f,
    };
}

float pid_step(PID* pid, float error, float dt) {
    float p = error;

    // Assumes delta_time is very small
    float d = (error - pid->_last_error) * dt;
    if (isnan(d) || isinf(d)) {
        d = 0.f;
    }

    // trapezoidal method for calculating integral
    pid->_integral += 0.5f * (error + pid->_last_error) * dt;
    float i = pid->_integral;

    pid->_last_error = error;

    return (pid->gain_proportional * p) + (pid->gain_integral * i) + (pid->gain_derivative * d);
}
