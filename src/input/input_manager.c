#define KEYBOARD_SCANCODE_PRIME 3637
#define MOUSE_BUTTON_PRIME 8527

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

void dispatch_keyboard_event(InputManager* manager, InputData keyboard_event) {
    int adjusted_scancode = keyboard_event.keyboard.scancode * KEYBOARD_SCANCODE_PRIME;
    update_input_state(manager, keyboard_event, adjusted_scancode);

    const Vector* callback_vector = hashmap_get(&manager->action_events, &keyboard_event.action);
    if (!callback_vector) { return; }
    _MetaInputState* state = (_MetaInputState*)hashmap_get(&manager->input_state, &adjusted_scancode);
    for (int i = 0; i < callback_vector->length; i++) {
        InputEvent event = _VECTOR_GET(InputEvent, callback_vector, i);
        switch (keyboard_event.state) {
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
                if (event.on_hold && state->last_state == INPUT_STATE_HOLDING) {
                    event.on_hold(event.user_data, keyboard_event);
                }
                break;
            case INPUT_STATE_RELEASE:
                if (event.on_release) {
                    event.on_release(event.user_data, keyboard_event);
                }
                break;
            default:
                log_debug("Keyboard state not handled: %d", keyboard_event.state);
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

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}

void report_key_released(InputManager* manager, Key key) {
    const void* action_str = hashmap_get(&manager->key_actions, &key.scancode);
    if (!action_str) { return; }

    InputData data;
    data.type = INPUT_TYPE_KEYBOARD;
    data.action = *(String*)action_str;
    data.state = INPUT_STATE_RELEASE;

    VECTOR_PUSH(InputData, &manager->queued_key_events, data);
}
