#ifndef CRASH_REASONS_H
#define CRASH_REASONS_H

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

#endif
