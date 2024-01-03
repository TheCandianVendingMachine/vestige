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
        .count = 7,
        .pairs = (struct IniDefaultValue[]) {
            (struct IniDefaultValue) { .key = "a.b.c.d.nested", .type = VALUE_TYPE_STRING, .value = { .string = "nested" } },
            (struct IniDefaultValue) { .key = "global", .type = VALUE_TYPE_STRING, .value = { .string = "global" } },
            (struct IniDefaultValue) { .key = "a.b.sub", .type = VALUE_TYPE_STRING, .value = { .string = "sub" } },
            (struct IniDefaultValue) { .key = "a.b.d.alt", .type = VALUE_TYPE_STRING, .value = { .string = "alt" } },
            (struct IniDefaultValue) { .key = "a.b.c.d.nested", .type = VALUE_TYPE_STRING, .value = { .string = "duplicate" } },
            (struct IniDefaultValue) { .key = "test.sub.bool", .type = VALUE_TYPE_BOOLEAN, .value = { .boolean = true } },
            (struct IniDefaultValue) { .key = "test.double", .type = VALUE_TYPE_NUMBER, .value = { .number = 3.14159 } },
        }
    };
    Time p0 = get_elapsed_time(&start);
    IniFile file = construct_ini_from_defaults(test_defaults);
    Time p1 = get_elapsed_time(&start);
    double d = get_number_from_ini(file, "test.double");
    Time p2 = get_elapsed_time(&start);
    const char* s = get_string_from_ini(file, "a.b.c.d.nested");
    const char* g = get_string_from_ini(file, "global");
    bool b = get_boolean_from_ini(file, "test.sub.bool");
    const char* incorrect = get_string_from_ini(file, "test.number");
    printf("double: %f\n", d);
    printf("boolean: %d\n", b);
    printf("string: %s\n", s);
    printf("global: %s\n", g);
    printf("incorrect: %s\n", incorrect);
    Time p3 = get_elapsed_time(&start);
    destroy_ini_file(file);
    Time p4 = get_elapsed_time(&start);


    printf("Construct time: %ld nanoseconds\nGet time: %ld nanoseconds\nDestroy Time: %ld nanoseconds\nTotal time: %ld nanoseconds", time_as_nanoseconds(time_elapsed(p0, p1)), time_as_nanoseconds(time_elapsed(p1, p2)), time_as_nanoseconds(time_elapsed(p3, p4)), time_as_nanoseconds(time_elapsed(p0, p4)));
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
