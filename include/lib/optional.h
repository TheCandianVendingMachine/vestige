#ifndef VESTIGE_OPTIONAL_H
#define VESTIGE_OPTIONAL_H

#include <stdlib.h>
#include "engine_lib.h"

typedef enum Option {
    OPTION_HAS_SOME,
    OPTION_HAS_NONE
} Option;

#define DEFINE_OPTION(T) \
typedef struct T##Option { \
    T data; \
    Option filled; \
} T##Option

#define OPTION_SOME(T, d) \
(T##Option) { \
    .data = d, \
    .filled = OPTION_HAS_SOME \
}
#define OPTION_NONE(T) \
(T##Option) { \
    .filled = OPTION_HAS_NONE \
}

#define MATCH_OPTION_SOME(o) if ((o).filled == OPTION_HAS_SOME)
#define MATCH_OPTION_NONE(o) if ((o).filled == OPTION_HAS_NONE)

#define OPTION_UNWRAP(T, o) ( \
    (o.filled == OPTION_HAS_NONE) ? \
        (*(T*)((void*)(engine_crash(SHUTDOWN_LIBRARY_ERROR)))) :\
        ((o).data) \
)

#define OPTION_ASSIGN(o) o.filled = OPTION_HAS_SOME; o.data
#define OPTION_UNWRAP_UNSAFE(o) o.data

#define ASSERT_OPTION_NONE(o) MATCH_OPTION_SOME(o) { engine_crash(SHUTDOWN_LIBRARY_ERROR) }
#define ASSERT_OPTION_SOME(o) MATCH_OPTION_NONE(o) { engine_crash(SHUTDOWN_LIBRARY_ERROR) }

#endif
