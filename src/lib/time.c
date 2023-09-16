// Needed for clock_gettime
#define _DEFAULT_SOURCE 1
// For nanosleep
#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "lib/time.h"
#include "logger.h"

#define MILLI   1000
#define MICRO   1000000
#define NANO    1000000000

Time time_add(Time lhs, Time rhs) {
    int64_t carry_second = (lhs._nanoseconds + rhs._nanoseconds) >= NANO;
    return (Time) {
        ._seconds = lhs._seconds + rhs._seconds + carry_second,
        ._nanoseconds = (lhs._nanoseconds + rhs._nanoseconds) % NANO
    };
}

Time time_sub(Time lhs, Time rhs) {
    int64_t carry_second = lhs._nanoseconds < rhs._nanoseconds;
    return (Time) {
        ._seconds = lhs._seconds - rhs._seconds - carry_second,
        ._nanoseconds = lhs._nanoseconds - rhs._nanoseconds + carry_second * NANO
    };
}

Time zero_time(void) {
    return (Time) {
        ._seconds = 0,
        ._nanoseconds = 0
    };
}

Time current_time(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    return (Time) {
        ._seconds = time.tv_sec,
        ._nanoseconds = time.tv_nsec
    };
}

Time time_elapsed(Time start, Time end) {
    return time_sub(end, start);
}

Time time_from_seconds(Seconds seconds) {
    Seconds subseconds = seconds - floorf(seconds);
    return (Time) {
        ._seconds = (int64_t)floorf(seconds),
        ._nanoseconds = (int64_t)(subseconds * NANO)
    };
}

Time time_from_milliseconds(Milliseconds milliseconds) {
    return (Time) {
        ._seconds = milliseconds / MILLI,
        ._nanoseconds = (milliseconds % MILLI) * MICRO
    };
}

Time time_from_microseconds(Microseconds microseconds) {
    return (Time) {
        ._seconds = microseconds / MICRO,
        ._nanoseconds = (microseconds % MICRO) * MILLI
    };
}

Time time_from_nanoseconds(Nanoseconds nanoseconds) {
    return (Time) {
        ._seconds = nanoseconds / NANO,
        ._nanoseconds = (nanoseconds % NANO)
    };
}

Seconds time_as_seconds(Time time) {
    return (Seconds)time._seconds + (Seconds)time._nanoseconds / (Seconds)NANO;
}

Milliseconds time_as_milliseconds(Time time) {
    return time._seconds * MILLI + time._nanoseconds / MICRO;
}

Microseconds time_as_microseconds(Time time) {
    return time._seconds * MICRO + time._nanoseconds / MILLI;
}

Nanoseconds time_as_nanoseconds(Time time) {
    return time._seconds * NANO + time._nanoseconds;
}

void thread_sleep_for(Time time) {
    struct timespec t = {
        .tv_sec = time._seconds,
        .tv_nsec = time._nanoseconds
    };
    nanosleep(&t, NULL);
}
