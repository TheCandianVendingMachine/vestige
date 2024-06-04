#ifndef VESTIGE_ENGINE_H
#define VESTIGE_ENGINE_H

#include <stdbool.h>

#include "engine_lib.h"
#include "window.h"
#include "lib/clock.h"
#include "lib/time.h"
#include "lib/vector.h"
#include "game/game_states.h"
#include "input/input_manager.h"
#include "ini.h"

typedef struct Simulation {
    Time _last_update_time;
    float _delta_time;
    float _accumulator;
} Simulation;

typedef struct GameManager {
    Clock game_clock;
    Vector _active_states;
    Vector _queued_states;
    int _pops_queued;
} GameManager;

typedef struct Inputs {
    InputManager manager;
    IniFile default_actions;
} Inputs;

typedef struct {
    Window window;
    unsigned int fps;
    // This clock runs for the duration of the program. Output is time since start
    Clock engine_clock;
    Simulation simulation;
    GameManager game;
    Inputs inputs;
    ShutdownReason shutdown_reason;
} Engine;

extern Engine* ENGINE;
extern bool ENGINE_RUNNING;

void engine_tick(void);

void engine_start(void);
int engine_stop(void);

void push_game_state(GameStateEnum state);
void queue_game_state_pop(void);

#endif
