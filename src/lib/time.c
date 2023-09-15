// Needed for clock_gettime
#define _DEFAULT_SOURCE 1

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "lib/time.h"

#define MILLI   1000
#define MICRO   1000000
#define NANO    1000000000

Time time_add(Time lhs, Time rhs) {
    uint64_t carry_second = (lhs._nanoseconds + rhs._nanoseconds) >= NANO;
    return (Time) {
        ._seconds = lhs._seconds + rhs._seconds + carry_second,
        ._nanoseconds = (lhs._nanoseconds + rhs._nanoseconds) % NANO
    };
}

Time time_sub(Time lhs, Time rhs) {
    uint64_t carry_second = rhs._nanoseconds > lhs._nanoseconds;
    return (Time) {
        ._seconds = lhs._seconds - rhs._seconds - carry_second,
        ._nanoseconds = (uint64_t)labs((int64_t)lhs._nanoseconds - (int64_t)rhs._nanoseconds)
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
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
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
        ._seconds = (uint64_t)floorf(seconds),
        ._nanoseconds = (uint64_t)(subseconds * NANO)
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
    return (Seconds)time._seconds + (Seconds)time._nanoseconds / (float)NANO;
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
