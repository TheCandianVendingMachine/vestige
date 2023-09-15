#include <stdlib.h>

#include "engine.h"
#include "logger.h"
#include "render.h"
#include "render/color.h"

#define DELTA_TIME (1.f / 100.f)

Engine* ENGINE = NULL;
bool ENGINE_RUNNING = false;

void engine_events(void) {
    glfwPollEvents();
}

void engine_update(void) {
    Time now = current_time();
    Time frame_time = time_elapsed(ENGINE->simulation._last_update_time, now);
    ENGINE->simulation._last_update_time = now;

    ENGINE->simulation._accumulator += time_as_seconds(frame_time);

    while (ENGINE->simulation._accumulator >= ENGINE->simulation._delta_time) {
        ENGINE->simulation._accumulator -= ENGINE->simulation._delta_time;
    }
}

void engine_render(void) {
    window_clear(&ENGINE->window);
    window_display(&ENGINE->window);
}

void engine_tick(void) {
    engine_events();
    engine_update();
    engine_render();
}

void graphics_init(void) {
    if (!glfwInit()) {
        engine_crash(SHUTDOWN_CANT_INIT_GLFW);
    }

    if (!create_window(&ENGINE->window)) {
        engine_crash(SHUTDOWN_CANT_INIT_WINDOW);
    }

    glfwMakeContextCurrent(ENGINE->window.window);

    initialise_renderer();

    ColorRGB clear_colour = hex_to_rgb("0xFFFF00");
    window_set_clear_color(&ENGINE->window, clear_colour);

    log_info("Graphics initalised");
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
    ENGINE->engine_clock = new_clock();

    graphics_init();

    ENGINE->simulation = (Simulation) {
        ._delta_time = DELTA_TIME,
        ._last_update_time = current_time(),
        ._accumulator = 0.f
    };
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
        log_error(ENGINE_NAME " shutdown. Reason: %s", SHUTDOWN_REASONS_STR[shutdown_return]);
    } else {
        log_info(ENGINE_NAME " shutdown. Reason: %s", SHUTDOWN_REASONS_STR[shutdown_return]);
    }
    logger_stop();

    ENGINE_RUNNING = false;
    return shutdown_return;
}
