#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "window.h"
#include "engine.h"
#include "render/texture.h"
#include "game/game_states.h"

int main(int argc, char* argv[]) {
    Vector program_args = VECTOR(const char*);
    for (int i = 0; i < argc; i++) {
        VECTOR_PUSH(const char*, &program_args, argv[i]);
    }
    engine_start();
    del_vector(program_args);
    push_game_state(GAME_STATE_GAMEPLAY);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(engine_tick, 0, 1);
#else
    while (!has_window_requested_close(&ENGINE->window)) {
        engine_tick();
    }
#endif

    glfwTerminate();

    return engine_stop();
}
