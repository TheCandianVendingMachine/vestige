#include <stdlib.h>

#include "game/bots/camera_controller.h"
#include "input/input_manager.h"
#include "render/camera.h"
#include "engine.h"
#include "logger.h"

void pan_camera(void* user_data, InputData input) {
    CameraController* controller = user_data;
    if (input.state == INPUT_STATE_PRESS) {
        controller->initial_click_pos = ENGINE->window.cursor.current_position;
        controller->initial_camera_pos.x = controller->camera->position.x;
        controller->initial_camera_pos.y = controller->camera->position.y;
    } else if (input.state == INPUT_STATE_HOLDING) {
        Vector2f offset = sub_vector2f(controller->initial_click_pos, ENGINE->window.cursor.current_position);
        Vector2f new_position = add_vector2f(controller->initial_camera_pos, offset);
        controller->camera->position.x = new_position.x;
        controller->camera->position.y = new_position.y;
        controller->camera->_updated = true;
    }
}

void zoom_camera(void* user_data, InputData input) {
    CameraController* controller = user_data;
    controller->current_zoom -= input.mouse.scroll * 5.f;
    if (controller->current_zoom <= 5.f) { controller->current_zoom = 5.f; }
    if (controller->current_zoom >= 250.f) { controller->current_zoom = 250.f; }

    controller->camera->zoom = 1.f / controller->current_zoom;
    controller->camera->_updated = true;
}

CameraController* create_camera_controller(struct Camera* camera) {
    CameraController* controller = malloc(sizeof(CameraController));
    controller->current_zoom = 10.f;
    controller->camera = camera;
    controller->camera->zoom = 1 / 10.f;
    controller->camera->_updated = true;

    InputEvent pan_event = (InputEvent) {
        .user_data = controller,
        .on_press = pan_camera,
        .on_hold = pan_camera
    };

    InputEvent zoom_event = (InputEvent) {
        .user_data = controller,
        .on_scroll = zoom_camera
    };

    register_action_event(&ENGINE->inputs.manager, "camera_pan", pan_event);
    register_action_event(&ENGINE->inputs.manager, "camera_zoom", zoom_event);

    return controller;
}
