#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "engine.h"
#include "logger.h"
#include "render/color.h"

Engine* ENGINE = NULL;
bool ENGINE_RUNNING = false;

void engine_tick(void) {
    window_display(&ENGINE->window);
    glfwPollEvents();
}

void graphics_init(void) {
    log_info("Graphics initalised");
    if (!glfwInit()) {
        engine_crash(SHUTDOWN_CANT_INIT_GLFW);
    }

    if (!create_window(&ENGINE->window)) {
        engine_crash(SHUTDOWN_CANT_INIT_WINDOW);
    }
    glfwMakeContextCurrent(ENGINE->window.window);

    ColorRGB clear_colour = { { 255, 255, 0 } };
    window_set_clear_color(&ENGINE->window, clear_colour);

    glfwSwapInterval(1);
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
    ENGINE_RUNNING = true;
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

    ENGINE_RUNNING = false;
    return shutdown_return;
}
