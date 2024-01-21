#include <math.h>

#include "game/bots/predictors.h"

PredictionResult linear_predictor(
    Vector2f origin, float speed,
    float predict_time_ahead,
    PredictionBody target
) {
    PredictionResult result;

    // Compare how long it takes for our bullet to get there vs target
    // Pick result with time difference being minimum
    // Integrate target acceleration to get velocity
    const int max_integration_steps = 1000;
    const float dt = predict_time_ahead / max_integration_steps;

    result.dt = predict_time_ahead;
    result.tti = 1e10;

    float t = 0.f;
    for (int j = 0; j < max_integration_steps; j++) {
        target.velocity = add_vector2f(target.velocity, mul_vector2f(target.acceleration, dt));
        target.position = add_vector2f(target.position, mul_vector2f(target.velocity, dt));

        float target_distance = distance_vector2f(origin, target.position);
        float bullet_tti = target_distance / speed;

        t += dt;

        float test_dt = fabs(t - bullet_tti);
        if (test_dt < result.dt && bullet_tti < result.tti) {
            result.tti = bullet_tti;
            result.target = target.position;
            result.dt = test_dt;
        }
    }

    return result;
}
