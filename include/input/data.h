#ifndef VESTIGE_INPUT_DATA_H
#define VESTIGE_INPUT_DATA_H

#include "lib/math.h"
#include "lib/string.h"
#include "input/key.h"

typedef enum InputState {
    INPUT_STATE_PRESS,
    INPUT_STATE_DOUBLE_PRESS,
    INPUT_STATE_HOLDING,
    INPUT_STATE_RELEASE
} InputState;

typedef struct InputData {
    String action;
    enum {
        INPUT_TYPE_KEYBOARD,
    } type;
    union {
        struct {
            InputState state;
        } keyboard;
    };
} InputData;

typedef struct InputEvent {
    void* user_data;
    void (*on_press)(void*, InputData);
    void (*on_release)(void*, InputData);
    void (*on_double_press)(void*, InputData);
    void (*on_hold)(void*, InputData);
} InputEvent;

#endif
