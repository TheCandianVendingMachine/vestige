#ifndef VESTIGE_INPUT_DATA_H
#define VESTIGE_INPUT_DATA_H

#include "lib/math.h"
#include "lib/string.h"
#include "input/key.h"

typedef enum InputState {
    INPUT_STATE_PRESS,
    INPUT_STATE_DOUBLE_PRESS,
    INPUT_STATE_HOLDING,
    INPUT_STATE_RELEASE,
    INPUT_STATE_SCROLL
} InputState;

typedef struct InputData {
    String action;
    InputState state;
    enum {
        INPUT_TYPE_KEYBOARD,
        INPUT_TYPE_MOUSE
    } type;
    union {
        struct {
            int scancode;
        } keyboard;
        struct {
            int button;
            float scroll;
        } mouse;
    };
} InputData;

typedef struct InputEvent {
    void* user_data;
    void (*on_press)(void*, InputData);
    void (*on_release)(void*, InputData);
    void (*on_double_press)(void*, InputData);
    void (*on_hold)(void*, InputData);
    void (*on_scroll)(void*, InputData);
} InputEvent;

#endif
