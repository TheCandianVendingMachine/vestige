#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include "window.h"
#include "lib/clock.h"
#include "lib/time.h"

#define ENGINE_NAME "Vestige"

typedef enum {
    SHUTDOWN_NORMAL = 0,
    SHUTDOWN_CANT_INIT_GLFW,
    SHUTDOWN_CANT_INIT_WINDOW,
} ShutdownReason;

static const char* SHUTDOWN_REASONS_STR[] = {
    "Normal Shutdown",
    "Can't initialise GLFW",
    "Can't initialise window"
};

typedef struct Simulation {
    Time _last_update_time;
    float _delta_time;
    float _accumulator;
} Simulation;

typedef struct {
    Window window;
    // This clock runs for the duration of the program. Output is time since start
    Clock engine_clock;
    Simulation simulation;
    ShutdownReason shutdown_reason;
} Engine;

extern Engine* ENGINE;
extern bool ENGINE_RUNNING;

void engine_tick(void);

void engine_start(void);
void engine_crash(ShutdownReason reason);
int engine_stop(void);

#endif
