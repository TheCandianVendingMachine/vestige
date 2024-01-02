#define KEYBOARD_SCANCODE_PRIME(k) ((k + 1) * 3637)
#define MOUSE_BUTTON_PRIME(b) ((b + 1) * 8527)
#define MOUSE_SCROLL_INPUT 1254739

#include <GLFW/glfw3.h>

#include "input/input_manager.h"
#include "lib/hashmap.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_INPUT
#include "logger.h"
#include "lib/vector.h"

typedef struct _MetaInputState {
    InputState current_state;
    InputState last_state;
    Time last_time_pressed;
} _MetaInputState;

InputManager initialise_input_manager(struct Window* window) {
    log_info("Creating input manager...");
    InputManager inputs;
    inputs.input_clock = new_clock();
    inputs.on_next_key_press = NULL;

    inputs.input_state = GHASHMAP(_MetaInputState, inthash);
    inputs.key_actions = GHASHMAP(String, inthash);
    inputs.mouse_actions = GHASHMAP(String, inthash);
    inputs.action_events = GHASHMAP(Vector, stringhash);
    inputs.queued_key_events = VECTOR(InputData);

    inputs.hold_queue = VECTOR(InputData);
    inputs.double_press_queue = VECTOR(InputData);

    inputs.double_press_time = time_from_seconds(0.2);

    return inputs;
}

void update_input_state(InputManager* manager, InputData event, int adjusted_input) {
    bool first_state = false;
    if (!hashmap_get(&manager->input_state, &adjusted_input)) {
        _MetaInputState state = (_MetaInputState) {
            .current_state = event.state
        };
        state.last_time_pressed = get_elapsed_time(&manager->input_clock);
        hashmap_set(&manager->input_state, &adjusted_input, &state);
        first_state = true;
    }
    _MetaInputState* state = (_MetaInputState*)hashmap_get(&manager->input_state, &adjusted_input);
    Time current_time = get_elapsed_time(&manager->input_clock);
    if (!first_state && event.state == INPUT_STATE_PRESS) {
        Time elapsed = time_elapsed(state->last_time_pressed, current_time);
        if (time_as_seconds(elapsed) <= time_as_seconds(manager->double_press_time)) {
            // queue a double press
            VECTOR_PUSH(InputData, &manager->double_press_queue, event);
        }
        state->last_time_pressed = get_elapsed_time(&manager->input_clock);
    }
    state->last_state = state->current_state;
    state->current_state = event.state;
    // If we have pressed the key, or we have released the key
    if (
        (state->last_state != INPUT_STATE_RELEASE && state->current_state == INPUT_STATE_HOLDING) ||
        state->current_state == INPUT_STATE_PRESS
    ) {
        // queue a hold
        VECTOR_PUSH(InputData, &manager->hold_queue, event);
    }
}

void dispatch_input_queue(InputManager* manager) {
    for (int i = 0; i < manager->queued_key_events.length; i++) {
        InputData input = _VECTOR_GET(InputData, &manager->queued_key_events, i);
        int adjusted_input = 0;
        switch (input.type) {
            case INPUT_TYPE_KEYBOARD:
                adjusted_input = KEYBOARD_SCANCODE_PRIME(input.keyboard.scancode);
                break;
            case INPUT_TYPE_MOUSE:
                adjusted_input = MOUSE_BUTTON_PRIME(input.mouse.button);
                break;
            default:
                log_debug("No dispatch method written for input type %d", input.type);
                break;
        };
        update_input_state(manager, input, adjusted_input);

        const Vector* callback_vector = hashmap_get(&manager->action_events, &input.action);
        if (!callback_vector) { return; }
        _MetaInputState* state = (_MetaInputState*)hashmap_get(&manager->input_state, &adjusted_input);
        for (int i = 0; i < callback_vector->length; i++) {
            InputEvent event = _VECTOR_GET(InputEvent, callback_vector, i);
            switch (input.state) {
                case INPUT_STATE_PRESS:
                    if (event.on_press) {
                        event.on_press(event.user_data, input);
                    }
                    break;
                case INPUT_STATE_DOUBLE_PRESS:
                    if (event.on_double_press) {
                        event.on_double_press(event.user_data, input);
                    }
                    break;
                case INPUT_STATE_HOLDING:
                    if (event.on_hold && state->last_state == INPUT_STATE_HOLDING) {
                        event.on_hold(event.user_data, input);
                    }
                    break;
                case INPUT_STATE_RELEASE:
                    if (event.on_release) {
                        event.on_release(event.user_data, input);
                    }
                    break;
                case INPUT_STATE_SCROLL:
                    if (event.on_scroll) {
                        event.on_scroll(event.user_data, input);
                    }
                    break;
                default:
                    log_debug("State not handled: %d", input.state);
                    break;
            }
        }
    }
    manager->queued_key_events.length = 0;

    for (int i = 0; i < manager->double_press_queue.length; i++) {
        InputData input = _VECTOR_GET(InputData, &manager->double_press_queue, i);
        input.state = INPUT_STATE_DOUBLE_PRESS;
        VECTOR_PUSH(InputData, &manager->queued_key_events, input);
    }
    manager->double_press_queue.length = 0;

    for (int i = 0; i < manager->hold_queue.length; i++) {
        InputData input = _VECTOR_GET(InputData, &manager->hold_queue, i);
        input.state = INPUT_STATE_HOLDING;
        VECTOR_PUSH(InputData, &manager->queued_key_events, input);
    }
    manager->hold_queue.length = 0;
}

void register_key_action(InputManager* manager, const char* action, Key key) {
    if (!key.scancode) {
        key.scancode = glfwGetKeyScancode(key.key);
    }

    String id = string_from_cstr((char*)action);
    hashmap_set(&manager->key_actions, &key.scancode, &id);
}

void register_mouse_action(InputManager* manager, const char* action, Button button) {
    String id = string_from_cstr((char*)action);
    hashmap_set(&manager->mouse_actions, &button.button, &id);
}

void register_scroll_action(InputManager* manager, const char* action) {
    String id = string_from_cstr((char*)action);
    int scroll_button = MOUSE_SCROLL_INPUT;
    hashmap_set(&manager->mouse_actions, &scroll_button, &id);
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
    data.state = INPUT_STATE_PRESS;
    data.keyboard.scancode = key.scancode;

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}

void report_key_released(InputManager* manager, Key key) {
    const void* action_str = hashmap_get(&manager->key_actions, &key.scancode);
    if (!action_str) { return; }

    InputData data;
    data.type = INPUT_TYPE_KEYBOARD;
    data.action = *(String*)action_str;
    data.state = INPUT_STATE_RELEASE;
    data.keyboard.scancode = key.scancode;

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}

void report_mouse_pressed(InputManager* manager, Button button) {
    const void* action_str = hashmap_get(&manager->mouse_actions, &button.button);
    if (!action_str) { return; }

    InputData data;
    data.type = INPUT_TYPE_MOUSE;
    data.action = *(String*)action_str;
    data.state = INPUT_STATE_PRESS;

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}

void report_mouse_released(InputManager* manager, Button button) {
    const void* action_str = hashmap_get(&manager->mouse_actions, &button.button);
    if (!action_str) { return; }

    InputData data;
    data.type = INPUT_TYPE_MOUSE;
    data.action = *(String*)action_str;
    data.state = INPUT_STATE_RELEASE;

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}

void report_mouse_scroll(InputManager* manager, float scroll) {
    int scroll_button = MOUSE_SCROLL_INPUT;
    const void* action_str = hashmap_get(&manager->mouse_actions, &scroll_button);
    if (!action_str) { return; }

    InputData data;
    data.type = INPUT_TYPE_MOUSE;
    data.action = *(String*)action_str;
    data.mouse.scroll = scroll;
    data.state = INPUT_STATE_SCROLL;

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}
