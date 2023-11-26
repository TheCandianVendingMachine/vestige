#ifndef VESTIGE_CAMERA_H
#define VESTIGE_CAMERA_H

#include <stdbool.h>

#include "lib/math.h"

typedef struct Camera {
    Vector3f position;
    Vector3f direction;
    Vector3f up;

    bool _updated;
    Matrix4f _projection;
} Camera;

Camera new_camera(void);
Matrix4f camera_view(Camera* camera);

#endif
