#ifndef TEST_INFO_H
#define TEST_INFO_H

#include <stdio.h>

#define TEST_ASSERT(b,i) if (i->status == INCONCLUSIVE && !b) { i->status = FAILURE; i->line = __LINE__; }

typedef enum TestStatus {
    INCONCLUSIVE,
    SUCCESS,
    FAILURE
} TestStatus;

const char* test_status_get_string(TestStatus status);

typedef struct TestInfo {
    TestStatus status;
    size_t line;
    char* test_output;
    size_t test_output_len;
    FILE* test_stream;
} TestInfo;

TestInfo new_test_info(void);

#endif
