#ifndef BOTS_PREDICTORS_H
#define BOTS_PREDICTORS_H

#include "lib/math.h"

typedef struct PredictionResult {
    Vector2f target;
    float tti;
    float dt;
} PredictionResult;

typedef struct PredictionBody {
    Vector2f position;
    Vector2f velocity;
    Vector2f acceleration;
} PredictionBody;

PredictionResult linear_predictor(Vector2f origin, float speed, float predict_time_ahead, PredictionBody target);

#endif
