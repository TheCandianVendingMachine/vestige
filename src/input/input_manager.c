#include <GLFW/glfw3.h>

#include "input/input_manager.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_INPUT
#include "logger.h"
#include "lib/vector.h"

InputManager initialise_input_manager(struct Window* window) {
    log_info("Creating input manager...");
    InputManager inputs;
    inputs.input_clock = new_clock();
    inputs.on_next_key_press = NULL;

    inputs.key_actions = GHASHMAP(String, inthash);
    inputs.action_events = GHASHMAP(Vector, stringhash);
    inputs.queued_key_events = VECTOR(InputData);

    return inputs;
}

void dispatch_keyboard_event(InputManager* manager, InputData keyboard_event) {
    const Vector* callback_vector = hashmap_get(&manager->action_events, &keyboard_event.action);
    if (!callback_vector) { return; }
    for (int i = 0; i < callback_vector->length; i++) {
        InputEvent event = _VECTOR_GET(InputEvent, callback_vector, i);
        switch (keyboard_event.keyboard.state) {
            case INPUT_STATE_PRESS:
                if (event.on_press) {
                    event.on_press(event.user_data, keyboard_event);
                }
                break;
            case INPUT_STATE_DOUBLE_PRESS:
                if (event.on_double_press) {
                    event.on_double_press(event.user_data, keyboard_event);
                }
                break;
            case INPUT_STATE_HOLDING:
                if (event.on_hold) {
                    event.on_hold(event.user_data, keyboard_event);
                }
                break;
            case INPUT_STATE_RELEASE:
                if (event.on_release) {
                    event.on_release(event.user_data, keyboard_event);
                }
                break;
            default:
                log_debug("Keyboard state not handled: %d", keyboard_event.keyboard.state);
                break;
        }
    }
}

void dispatch_input_queue(InputManager* manager) {
    for (int i = 0; i < manager->queued_key_events.length; i++) {
        InputData input = _VECTOR_GET(InputData, &manager->queued_key_events, i);
        switch (input.type) {
            case INPUT_TYPE_KEYBOARD:
                dispatch_keyboard_event(manager, input);
                break;
            default:
                log_debug("No dispatch method written for input type %d", input.type);
                break;
        };
    }
    manager->queued_key_events.length = 0;
}

void register_key_action(InputManager* manager, const char* action, Key key) {
    if (!key.scancode) {
        key.scancode = glfwGetKeyScancode(key.key);
    }

    String id = string_from_cstr((char*)action);
    hashmap_set(&manager->key_actions, &key.scancode, &id);
}

void register_action_event(InputManager* manager, const char* action, InputEvent event) {
    String action_str = string_from_cstr((char*)action);
    if (!hashmap_get(&manager->action_events, &action_str)) {
        Vector action_map = VECTOR(InputEvent);
        hashmap_set(&manager->action_events, &action_str, &action_map);
    }

    Vector* action_map = (Vector*)hashmap_get(&manager->action_events, &action_str);
    VECTOR_PUSH(InputEvent, action_map, event);
}

void report_key_pressed(InputManager* manager, Key key) {
    const void* action_str = hashmap_get(&manager->key_actions, &key.scancode);
    if (!action_str) { return; }

    InputData data;
    data.type = INPUT_TYPE_KEYBOARD;
    data.action = *(String*)action_str;
    data.keyboard.state = INPUT_STATE_PRESS;

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}

void report_key_released(InputManager* manager, Key key) {
    const void* action_str = hashmap_get(&manager->key_actions, &key.scancode);
    if (!action_str) { return; }

    InputData data;
    data.type = INPUT_TYPE_KEYBOARD;
    data.action = *(String*)action_str;
    data.keyboard.state = INPUT_STATE_RELEASE;

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}
