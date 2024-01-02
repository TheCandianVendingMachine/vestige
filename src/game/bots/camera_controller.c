#include "game/bots/camera_controller.h"

enum {
    CAMERA_STATE_IDLE,
    CAMERA_STATE_DRAGGING
};

CameraController create_camera_controller(struct Camera* camera) {
    CameraController controller;
    controller.camera = camera;

    controller.state = CAMERA_STATE_IDLE;

    return controller;
}

void update(CameraController* controller) {

}
