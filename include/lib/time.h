#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <time.h>

typedef float       Seconds;
typedef uint64_t    Milliseconds;
typedef uint64_t    Microseconds;
typedef uint64_t    Nanoseconds;

typedef struct Time {
    uint64_t _seconds;
    Nanoseconds _nanoseconds;
} Time;

Time time_add(Time lhs, Time rhs);
Time time_sub(Time lhs, Time rhs);

Time zero_time(void);
Time current_time(void);
Time time_elapsed(Time start, Time end);

Time time_from_seconds(Seconds seconds);
Time time_from_milliseconds(Milliseconds milliseconds);
Time time_from_microseconds(Microseconds microseconds);
Time time_from_nanoseconds(Nanoseconds nanoseconds);

Seconds time_as_seconds(Time time);
Milliseconds time_as_milliseconds(Time time);
Microseconds time_as_microseconds(Time time);
Nanoseconds time_as_nanoseconds(Time time);

#endif
