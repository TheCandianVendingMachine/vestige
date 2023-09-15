#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "engine.h"
#include "logger.h"

Engine* ENGINE = NULL;

void graphics_init(void) {
    log_info("Graphics initalised");
    if (!glfwInit()) {
        engine_crash(SHUTDOWN_CANT_INIT_GLFW);
    }

    if (!create_window(&ENGINE->window)) {
        engine_crash(SHUTDOWN_CANT_INIT_WINDOW);
    }

    glfwMakeContextCurrent(ENGINE->window.window);
}

void graphics_deinit(void) {
    destroy_window(&ENGINE->window);
    glfwTerminate();
    log_info("Graphics deinitialised");
}

void engine_start(void) {
    logger_start();

    ENGINE = malloc(sizeof(Engine));
    ENGINE->shutdown_reason = SHUTDOWN_NORMAL;

    graphics_init();

    log_info(ENGINE_NAME " started");
}

void engine_crash(ShutdownReason reason) {
    ENGINE->shutdown_reason = reason;
    engine_stop();
    log_error("Engine crashed!");
    abort();
}

int engine_stop(void) {
    graphics_deinit();

    ShutdownReason shutdown_return = ENGINE->shutdown_reason;
    free(ENGINE);
    ENGINE = NULL;

    if (shutdown_return != SHUTDOWN_NORMAL) {
        log_warning(ENGINE_NAME " shutdown. Reason: %s", SHUTDOWN_REASONS_STR[shutdown_return]);
    } else {
        log_info(ENGINE_NAME " shutdown. Reason: %s", SHUTDOWN_REASONS_STR[shutdown_return]);
    }
    logger_stop();
    return shutdown_return;
}
