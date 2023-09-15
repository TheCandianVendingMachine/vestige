#ifndef RESULT_H
#define RESULT_H

#include "lib/string.h"

typedef enum Result {
    RESULT_OK,
    RESULT_ERROR
} Result;

#define DEFINE_RESULT(T) \
typedef struct T##Result { \
    union { \
        void* _none; \
        String error; \
        T ok; \
    } data; \
    Result result; \
} T##Result

#define OK_RESULT(T, d) \
(T##Result) { \
    .data = { .ok = d }, \
    .result = RESULT_OK \
}

#define ERR_RESULT(T, message) \
(T##Result) { \
    .data = { .error = string_from_cstr(message) }, \
    .result = RESULT_ERROR \
}

#define CONSUME_RESULT(T, r) \
    if (r.result == RESULT_ERROR) { del_string(r.data.error); } \
    r = (T##Result) { .data = { ._none = 0 }, .result = -1 }

#endif
