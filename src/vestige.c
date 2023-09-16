#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "window.h"
#include "engine.h"
#include "render/texture.h"

int main(int argc, char* argv[]) {
    engine_start();

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
