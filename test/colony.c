#include <stdio.h>
#include <lib/colony.h>
#include <lib/clock.h>

#include "test_info.h"

void test_colony(void* complete) {
    TestInfo* info = (TestInfo*)complete;

    const int TEST_COUNT = 10000;
    const int TEST_MAX = (1 << 17);

    Time avg_elapsed_time = zero_time();
    Time avg_insert_time = zero_time();
    Time avg_remove_time = zero_time();
    Time avg_reinsert_time = zero_time();
    Time avg_iterate_time = zero_time();

    for (int i = 0; i < TEST_COUNT; i++) {
        Colony colony = new_colony(sizeof(int));

        Clock timer = new_clock();
        Time start = get_elapsed_time(&timer);
        // insert
        for (int i = 0; i < TEST_MAX; i++) {
            colony_insert(&colony, &i);
        }
        Time c1 = get_elapsed_time(&timer);

        // remove
        for (int i = 1; i < TEST_MAX; i += 2) {
            colony_remove(&colony, i);
        }
        Time c2 = get_elapsed_time(&timer);

        // reinsert
        for (int i = TEST_MAX; i < TEST_MAX * 2; i++) {
            colony_insert(&colony, &i);
        }

        Time c3 = get_elapsed_time(&timer);
        // iterate
#ifndef COLONY_TEST_NO_CPY
        COLONY_ITER_BEGIN(int, colony);
        *i *= 2;
        COLONY_ITER_END;
#endif
        Time end = get_elapsed_time(&timer);

        avg_insert_time = time_add(avg_insert_time, time_elapsed(start, c1));
        avg_remove_time = time_add(avg_remove_time, time_elapsed(c1, c2));
        avg_reinsert_time = time_add(avg_reinsert_time, time_elapsed(c2, c3));
        avg_iterate_time = time_add(avg_iterate_time, time_elapsed(c3, end));
        avg_elapsed_time = time_add(avg_elapsed_time, time_elapsed(start, end));

        del_colony(colony);

        if (i % 500 == 0) {
            fprintf(info->test_stream, "%.2f%%\n", 100.f * (float)i / TEST_COUNT);
        }
    }

    fprintf(info->test_stream, "Stats for %d items:\n", TEST_MAX);
    fprintf(info->test_stream, "Average Total Time: %ld microseconds\n", time_as_microseconds(avg_elapsed_time) / TEST_COUNT);
    fprintf(info->test_stream, "Average Insert Time: %ld microseconds\n", time_as_microseconds(avg_insert_time) / TEST_COUNT);
    fprintf(info->test_stream, "Average Remove Time: %ld microseconds\n", time_as_microseconds(avg_remove_time) / TEST_COUNT);
    fprintf(info->test_stream, "Average Re-insert Time: %ld microseconds\n", time_as_microseconds(avg_reinsert_time) / TEST_COUNT);
    fprintf(info->test_stream, "Average Iterate Time: %ld microseconds\n", time_as_microseconds(avg_iterate_time) / TEST_COUNT);

    info->status = SUCCESS;
}
