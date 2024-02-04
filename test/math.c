#include <stdio.h>

#include <lib/math.h>

#include "test_info.h"

void test_math(void* complete) {
    TestInfo* info = (TestInfo*)complete;
    for (int i = 0; i < 100; i++) {
        fprintf(info->test_stream, "%.08f\n", frandrange(-1.1, 1.5));
    }
    for (int i = 0; i < 100; i++) {
        fprintf(info->test_stream, "%d\n", randint());
    }

    info->status = SUCCESS;
}
