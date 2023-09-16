#include <stdlib.h>

#include "engine.h"
#include "logger.h"
#include "render.h"
#include "render/color.h"
#include "game/game_states.h"

#define DELTA_TIME (1.f / 100.f)

Engine* ENGINE = NULL;
bool ENGINE_RUNNING = false;

void engine_events(void) {
    glfwPollEvents();
}

void engine_preupdate(void) {
    while (ENGINE->game._pops_queued-- > 0) {
        GameState top_state;
        VECTOR_POP(GameState, &ENGINE->game._active_states, &top_state);
        log_debug("Popping state %d", top_state.state_type);
        game_state_on_deinit(&top_state);
        game_state_on_pop(&top_state);
    }

    while (ENGINE->game._queued_states.length > 0) {
        GameStateEnum queued_top_state;
        VECTOR_POP(GameStateEnum, &ENGINE->game._queued_states, &queued_top_state);
        log_debug("Pushing state %d", queued_top_state);

        int active_state_length = ENGINE->game._active_states.length;
        GameState top_state;
        VECTOR_GET(GameState, &ENGINE->game._active_states, active_state_length - 1, &top_state);
        game_state_on_deinit(&top_state);

        VECTOR_PUSH(GameState, &ENGINE->game._active_states, (GameState) { .state_type = queued_top_state });
        // active_state_length will not have to be offset since we just pushed
        VECTOR_GET(GameState, &ENGINE->game._active_states, active_state_length, &top_state);
        game_state_on_push(&top_state);
        game_state_on_init(&top_state);
    }
}

void engine_update(void) {
    Time now = current_time();
    Time frame_time = time_elapsed(ENGINE->simulation._last_update_time, now);
    ENGINE->simulation._last_update_time = now;

    ENGINE->simulation._accumulator += time_as_seconds(frame_time);

    GameState top_state;
    VECTOR_GET(GameState, &ENGINE->game._active_states, (int)ENGINE->game._active_states.length - 1, &top_state);
    while (ENGINE->simulation._accumulator >= ENGINE->simulation._delta_time) {
        game_state_update(&top_state, ENGINE->simulation._delta_time);
        ENGINE->simulation._accumulator -= ENGINE->simulation._delta_time;
    }
}

void engine_render(void) {
    GameState top_state;
    VECTOR_GET(GameState, &ENGINE->game._active_states, (int)ENGINE->game._active_states.length - 1, &top_state);
    window_clear(&ENGINE->window);
    game_state_render(&top_state);
    window_display(&ENGINE->window);
}

void engine_tick(void) {
    engine_events();
    engine_preupdate();
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

    glfwSwapInterval(0);

    log_info("Graphics initalised");
}

void graphics_deinit(void) {
    destroy_window(&ENGINE->window);
    glfwTerminate();
    log_info("Graphics deinitialised");
}

void engine_start(void) {
    logger_start();
    Time engine_start_time = current_time();
    log_info("Starting" ENGINE_NAME "....");

    ENGINE = malloc(sizeof(Engine));
    ENGINE->shutdown_reason = SHUTDOWN_NORMAL;
    ENGINE->engine_clock = new_clock();
    log_info("Core started");


    graphics_init();

    ENGINE->simulation = (Simulation) {
        ._delta_time = DELTA_TIME,
        ._last_update_time = current_time(),
        ._accumulator = 0.f
    };
    log_info("Simulation started");
    log_info(ENGINE_NAME " started");

    ENGINE->game = (GameManager) {
        .game_clock = new_clock(),
        ._active_states = VECTOR(GameStateEnum),
        ._queued_states = VECTOR(GameStateEnum),
        ._pops_queued = 0
    };
    VECTOR_PUSH(GameStateEnum, &ENGINE->game._active_states, GAME_STATE_TOMBSTONE);

    log_info("Game manager started");
    ENGINE_RUNNING = true;
    Time engine_end_time = current_time();
    log_info("Startup took %.2f seconds", time_as_seconds(time_elapsed(engine_start_time, engine_end_time)));
}

void engine_crash(ShutdownReason reason) {
    ENGINE->shutdown_reason = reason;
    engine_stop();
    log_error("Engine crashed!");
    abort();
}

int engine_stop(void) {
    if (!ENGINE_RUNNING) {
        return -1;
    }

    while (ENGINE->game._active_states.length > 0) {
        GameState top_state;
        VECTOR_POP(GameState, &ENGINE->game._active_states, &top_state);
        game_state_on_deinit(&top_state);
        game_state_on_pop(&top_state);
    }
    del_vector(ENGINE->game._active_states);
    del_vector(ENGINE->game._queued_states);
    log_info("Shutdown game manager");

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

void push_game_state(GameStateEnum state) {
    log_debug("Queued push of state %d", state);
    VECTOR_PUSH(GameStateEnum, &ENGINE->game._queued_states, state);
}

void queue_game_state_pop(void) {
    ENGINE->game._pops_queued++;
    log_debug("Queued %d pop(s) next frame", ENGINE->game._pops_queued);
}
