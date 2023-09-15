#ifndef ENGINE_H
#define ENGINE_H

#define ENGINE_NAME "Vestige"

#include "shutdown_reasons.h"
#include "window.h"

typedef struct {
    Window window;
    ShutdownReason shutdown_reason;
} Engine;

extern Engine* ENGINE;

void engine_start(void);
void engine_crash(ShutdownReason reason);
int engine_stop(void);

#endif
