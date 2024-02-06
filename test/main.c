#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <lib/clock.h>

#include "tests.h"
#include "test_info.h"

enum TestState {
    RUNNING,
    REPORT,
    DONE
};

struct Test {
    const char* name;
    pthread_t thread;
    enum TestState state;
    TestInfo test_data;
};

struct Test run_test(const char* name, void* (*test)(void*)) {
    struct Test ret;
    ret.test_data = new_test_info();
    ret.name = name;
    ret.state = RUNNING;
    pthread_create(&ret.thread, NULL, test, &ret.test_data);
    return ret;
}

int main(void) {
    struct Test tests[] = {
        run_test("hashmap", test_hashmap),
        //run_test("colony", test_colony),
        run_test("vector", test_vector),
        run_test("math", test_math),
        run_test("vector (linalg)", test_vec)
    };

    Clock test_start = new_clock();
    size_t test_count = sizeof(tests) / sizeof(struct Test);
    size_t tests_done = 0;
    while (tests_done != test_count) {
        tests_done = 0;
        for (size_t i = 0; i < test_count; i++) {
            switch (tests[i].state) {
                case RUNNING:
                    if (tests[i].test_data.status != INCONCLUSIVE) {
                        tests[i].state = REPORT;
                    }
                    break;
                case REPORT:
                    pthread_join(tests[i].thread, NULL);
                    printf(
                        "Test '%s' complete with status: %s",
                        tests[i].name, 
                        test_status_get_string(tests[i].test_data.status)
                    );

                    if (tests[i].test_data.status == FAILURE) {
                        printf(" (Line %jd)\n", tests[i].test_data.line);
                        printf("\tTest Output: %s", tests[i].test_data.test_output);
                    }
                    printf("\n");

                    fclose(tests[i].test_data.test_stream);
                    free(tests[i].test_data.test_output);
                    tests[i].state = DONE;
                    break;
                case DONE:
                    tests_done++;
                    break;
            }
        }
    }

    Time elapsed = get_elapsed_time(&test_start);
    printf("All tests complete in %.2f seconds\n", time_as_seconds(elapsed));
}
