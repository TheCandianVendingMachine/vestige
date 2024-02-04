#ifndef TEST_INFO_H
#define TEST_INFO_H

#include <stdio.h>

#define TEST_ASSERT(b,i) if (!b) { i->status = FAILURE; }

typedef enum TestStatus {
    INCONCLUSIVE,
    SUCCESS,
    FAILURE
} TestStatus;

typedef struct TestInfo {
    TestStatus status;
    char* test_output;
    size_t test_output_len;
    FILE* test_stream;
} TestInfo;

TestInfo new_test_info(void);

#endif
