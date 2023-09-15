#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include "shutdown_reasons.h"
#include "window.h"

#define ENGINE_NAME "Vestige"

typedef struct {
    Window window;
    ShutdownReason shutdown_reason;
} Engine;

extern Engine* ENGINE;
extern bool ENGINE_RUNNING;

void engine_tick(void);

void engine_start(void);
void engine_crash(ShutdownReason reason);
int engine_stop(void);

#endif
