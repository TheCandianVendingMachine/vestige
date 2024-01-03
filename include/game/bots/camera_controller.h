#ifndef BOTS_CAMERA_CONTROLLER_H
#define BOTS_CAMERA_CONTROLLER_H

#include "lib/math.h"

struct Camera;

typedef struct CameraController {
    struct Camera* camera;
    struct {
        Vector2f initial_camera_pos;
        Vector2f initial_click_pos;
        float current_zoom;
        unsigned int state;
    };
} CameraController;

CameraController* create_camera_controller(struct Camera* camera);

#endif
