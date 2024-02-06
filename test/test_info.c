#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>

#include "test_info.h"

const char* test_status_get_string(TestStatus status) {
    switch (status) {
        case INCONCLUSIVE:
            return "Inconclusive";
        case SUCCESS:
            return "Success";
        case FAILURE:
            return "Failure";
        default:
            return "Unknown";
    }
}

TestInfo new_test_info(void) {
    TestInfo info;
    info.status = INCONCLUSIVE;
    info.test_output = calloc(256, sizeof(char));
    info.test_output_len = 0;
    info.test_stream = open_memstream(&info.test_output, &info.test_output_len);
    return info;
}
