#include <stdlib.h>
#include <math.h>

#include "engine.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_ENGINE
#include "logger.h"
#include "render.h"
#include "render/color.h"
#include "game/game_states.h"
#include "default_inputs.h"

#define DELTA_TIME (1.f / 100.f)
#define DEFAULT_ENGINE_FPS 60

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
        GameState *top_state;
        top_state = &_VECTOR_GET(GameState, &ENGINE->game._active_states, active_state_length - 1);
        log_debug("Deinitializing state %d", top_state->state_type);
        game_state_on_deinit(top_state);

        VECTOR_PUSH(GameState, &ENGINE->game._active_states, (GameState) { .state_type = queued_top_state });
        log_debug("%d pushed", queued_top_state);
        // active_state_length will not have to be offset since we just pushed
        top_state = &_VECTOR_GET(GameState, &ENGINE->game._active_states, active_state_length);
        game_state_on_push(top_state);
        game_state_on_init(top_state);
        log_debug("State %d initialised and ready", queued_top_state);
    }

    dispatch_input_queue(&ENGINE->inputs.manager);
}

void engine_update(void) {
    Time now = current_time();
    Time frame_time = time_elapsed(ENGINE->simulation._last_update_time, now);
    ENGINE->simulation._last_update_time = now;

    ENGINE->simulation._accumulator += time_as_seconds(frame_time);

    GameState *top_state;
    top_state = &_VECTOR_GET(GameState, &ENGINE->game._active_states, (int)ENGINE->game._active_states.length - 1);

    game_state_update(top_state);
    while (ENGINE->simulation._accumulator >= ENGINE->simulation._delta_time) {
        game_state_fixed_update(top_state, ENGINE->simulation._delta_time);
        ENGINE->simulation._accumulator -= ENGINE->simulation._delta_time;
    }
    game_state_post_update(top_state);
}

void engine_render(void) {
    GameState *top_state;
    top_state = &_VECTOR_GET(GameState, &ENGINE->game._active_states, (int)ENGINE->game._active_states.length - 1);
    window_clear(&ENGINE->window);
    game_state_render(top_state);
    window_display(&ENGINE->window);
}

void engine_tick(void) {
    Time tick_start = current_time();
    engine_events();
    engine_preupdate();
    engine_update();
    engine_render();
    Time tick_end = current_time();

    if (ENGINE->fps != 0) {
        Nanoseconds frame_time_nanoseconds = time_as_nanoseconds(time_elapsed(tick_start, tick_end));
        const Nanoseconds target_frame_time = (uint64_t)(1e9 / (double)ENGINE->fps);
        if (frame_time_nanoseconds <= target_frame_time) {
            // This may not sleep for the right time because of the overhead of the sleep
            // function used.
            const Nanoseconds sleep_time_nanoseconds = target_frame_time - frame_time_nanoseconds;
            thread_sleep_for(time_from_nanoseconds(sleep_time_nanoseconds));
        }
    }
    /*
        1/f == seconds/frame
        1/f * 1e6 == microseconds/frame
        1e6/f == microseconds/frame

        frame_time + sleep_time = target_frame_time
    */
}

void graphics_init(void) {
    if (!glfwInit()) {
        engine_crash(SHUTDOWN_CANT_INIT_GLFW);
    }
    log_info("GLFW initialised");

    if (!create_window(&ENGINE->window)) {
        engine_crash(SHUTDOWN_CANT_INIT_WINDOW);
    }
    log_info("Window created");

    glfwMakeContextCurrent(ENGINE->window.window);

    initialise_renderer();

    ColorRGB clear_colour = hex_to_rgb("0x333333");
    //ColorRGB clear_colour = hex_to_rgb("0xFFFFFF");
    window_set_clear_color(&ENGINE->window, clear_colour);

    glfwSwapInterval(0);

    log_info("Graphics initalised");
}

void graphics_deinit(void) {
    deinitialise_renderer();

    destroy_window(&ENGINE->window);
    log_info("Window destroyed");
    glfwTerminate();
    log_info("Graphics deinitialised");
}

void input_init(void) {
    ENGINE->inputs.manager = initialise_input_manager(&ENGINE->window);
    log_info("Reading inputs...");
    ENGINE->inputs.default_actions = construct_ini_from_defaults(VESTIGE_DEFAULT_INPUTS);

    log_info("Input system started");
}

void input_deinit(void) {
    destroy_ini_file(ENGINE->inputs.default_actions);
    log_info("Input system shutdown");
}

void engine_start(void) {
    logger_start();
    Time engine_start_time = current_time();
    log_info("Starting " ENGINE_NAME "....");

    ENGINE = malloc(sizeof(Engine));
    ENGINE->shutdown_reason = SHUTDOWN_NORMAL;
    ENGINE->engine_clock = new_clock();
    ENGINE->fps = DEFAULT_ENGINE_FPS;
    log_info("Engine core started");

    input_init();
    graphics_init();

    ENGINE->simulation = (Simulation) {
        ._delta_time = DELTA_TIME,
        ._last_update_time = current_time(),
        ._accumulator = 0.f
    };
    log_info("Simulation started");

    ENGINE->game = (GameManager) {
        .game_clock = new_clock(),
        ._active_states = VECTOR(GameStateEnum),
        ._queued_states = VECTOR(GameStateEnum),
        ._pops_queued = 0
    };
    VECTOR_PUSH(GameStateEnum, &ENGINE->game._active_states, GAME_STATE_TOMBSTONE);
    log_info("Game manager started");

    ENGINE_RUNNING = true;
    log_info(ENGINE_NAME " started");
    Time engine_end_time = current_time();
    log_info("Startup took %.2f seconds", time_as_seconds(time_elapsed(engine_start_time, engine_end_time)));
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
