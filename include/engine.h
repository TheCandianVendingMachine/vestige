#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include "window.h"

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

typedef struct {
    Window window;
    float _delta_time;
    ShutdownReason shutdown_reason;
} Engine;

extern Engine* ENGINE;
extern bool ENGINE_RUNNING;

void engine_tick(void);

void engine_start(void);
void engine_crash(ShutdownReason reason);
int engine_stop(void);

#endif
