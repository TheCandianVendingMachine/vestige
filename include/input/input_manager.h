#ifndef VESTIGE_INPUT_MANAGER_H
#define VESTIGE_INPUT_MANAGER_H

#include "lib/clock.h"
#include "lib/time.h"
#include "lib/string.h"
#include "lib/hashmap.h"
#include "lib/vector.h"
#include "lib/math.h"
#include "input/key.h"
#include "input/button.h"
#include "input/data.h"
#include "ini.h"

typedef struct InputManager {
    Clock input_clock;
    Time double_press_time;
    HashMap input_state;
    Vector hold_queue;
    Vector double_press_queue;
    HashMap key_actions;
    HashMap mouse_actions;
    HashMap action_events;
    Vector queued_key_events;
    void (*on_next_key_press)(Key, InputData);
} InputManager;

struct Window;
InputManager initialise_input_manager(struct Window* window);

void dispatch_input_queue(InputManager* manager);

void register_key_action(InputManager* manager, const char* action, Key key);
void register_mouse_action(InputManager* manager, const char* action, Button button);
void register_scroll_action(InputManager* manager, const char* action);

void register_action_event(InputManager* manager, const char* action, InputEvent event);

void report_key_pressed(InputManager* manager, Key key);
void report_key_released(InputManager* manager, Key key);
void report_mouse_pressed(InputManager* manager, Button button);
void report_mouse_released(InputManager* manager, Button button);
void report_mouse_scroll(InputManager* manager, float scroll);

#endif
