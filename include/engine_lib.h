#ifndef VESTIGE_ENGINE_LIB_H
#define VESTIGE_ENGINE_LIB_H

#include <stdint.h>

#define ENGINE_NAME "Vestige"

typedef enum {
    SHUTDOWN_NORMAL = 0,
    SHUTDOWN_CANT_INIT_GLFW,
    SHUTDOWN_CANT_INIT_GLAD,
    SHUTDOWN_CANT_INIT_WINDOW,
    SHUTDOWN_CANT_INIT_FREETYPE,
    SHUTDOWN_LIBRARY_ERROR
} ShutdownReason;

static const char* SHUTDOWN_REASONS_STR[] = {
    "Normal Shutdown",
    "Can't initialise GLFW",
    "Can't initialise GLAD",
    "Can't initialise window",
    "Can't initialise Freetype",
    "Error within library code"
};

uintptr_t impl_engine_crash(const char* file, int line, ShutdownReason reason);
#define engine_crash(reason) impl_engine_crash(__FILE__, __LINE__, reason)

#endif
