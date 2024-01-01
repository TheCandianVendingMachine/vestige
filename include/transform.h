#ifndef VESTIGE_TRANSFORM_H
#define VESTIGE_TRANSFORM_H

#include <stdbool.h>

#include "lib/math.h"

typedef struct Transform {
    Vector3f position;
    Vector3f size;
    Matrix4f _transform;
    bool _updated;
} Transform;

Transform new_transform(void);
void transform_set_position(Transform* transform, Vector3f position);
void transform_set_size(Transform* transform, Vector3f size);
Matrix4f transform_get_model(Transform* transform);

#endif
