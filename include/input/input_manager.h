#ifndef VESTIGE_INPUT_MANAGER_H
#define VESTIGE_INPUT_MANAGER_H

#include "lib/clock.h"
#include "lib/string.h"
#include "lib/hashmap.h"
#include "lib/vector.h"
#include "lib/math.h"
#include "input/key.h"
#include "input/data.h"

typedef struct InputManager {
    Clock input_clock;
    HashMap key_actions;
    HashMap action_events;
    Vector queued_key_events;
    void (*on_next_key_press)(Key, InputData);
} InputManager;

struct Window;
InputManager initialise_input_manager(struct Window* window);

void dispatch_input_queue(InputManager* manager);

void register_key_action(InputManager* manager, const char* action, Key key);
void register_action_event(InputManager* manager, const char* action, InputEvent event);

void report_key_pressed(InputManager* manager, Key key);
void report_key_released(InputManager* manager, Key key);

#endif
