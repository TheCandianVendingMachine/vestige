#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include "window.h"
#include "lib/clock.h"
#include "lib/time.h"
#include "lib/vector.h"
#include "game/game_states.h"

#define ENGINE_NAME "Vestige"

typedef enum {
    SHUTDOWN_NORMAL = 0,
    SHUTDOWN_CANT_INIT_GLFW,
    SHUTDOWN_CANT_INIT_GLAD,
    SHUTDOWN_CANT_INIT_WINDOW,
    SHUTDOWN_CANT_INIT_FREETYPE
} ShutdownReason;

static const char* SHUTDOWN_REASONS_STR[] = {
    "Normal Shutdown",
    "Can't initialise GLFW",
    "Can't initialise GLAD",
    "Can't initialise window",
    "Can't initialise Freetype"
};

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

typedef struct {
    Window window;
    unsigned int fps;
    // This clock runs for the duration of the program. Output is time since start
    Clock engine_clock;
    Simulation simulation;
    GameManager game;
    ShutdownReason shutdown_reason;
} Engine;

extern Engine* ENGINE;
extern bool ENGINE_RUNNING;

void engine_tick(void);

void engine_start(void);
void engine_crash(ShutdownReason reason);
int engine_stop(void);

void push_game_state(GameStateEnum state);
void queue_game_state_pop(void);

#endif
