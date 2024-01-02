#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "window.h"
#include "engine.h"
#include "render/texture.h"
#include "game/game_states.h"

#include "ini.h"
#include "lib/clock.h"

int main(int argc, char* argv[]) {
    Clock start = new_clock();
    IniDefault test_defaults = (IniDefault) {
        .count = 5,
        .pairs = (struct IniDefaultValue[]) {
            (struct IniDefaultValue) { .key = "a.b.c.d.nested", .value = { .string = "nested" } },
            (struct IniDefaultValue) { .key = "global", .value = { .string = "global" } },
            (struct IniDefaultValue) { .key = "a.b.sub", .value = { .string = "sub" } },
            (struct IniDefaultValue) { .key = "a.b.d.alt", .value = { .string = "alt" } },
            (struct IniDefaultValue) { .key = "a.b.c.d.nested", .value = { .string = "duplicate" } }
        }
    };
    Time p0 = get_elapsed_time(&start);
    IniFile file = construct_ini_from_defaults(test_defaults);
    Time p1 = get_elapsed_time(&start);
    destroy_ini_file(file);
    Time p2 = get_elapsed_time(&start);

    printf("File construct time: %ld nanoseconds\nDestroy time: %ld nanoseconds\nTotal time: %ld nanoseconds", time_as_nanoseconds(time_elapsed(p0, p1)), time_as_nanoseconds(time_elapsed(p1, p2)), time_as_nanoseconds(time_elapsed(p0, p2)));
    return 0;
    engine_start();
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
