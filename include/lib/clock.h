#ifndef CLOCK_H
#define CLOCK_H

#include <stdbool.h>

#include "lib/time.h"

typedef struct Clock {
    Time _start;
    Time _stop;
    Time _offset;
    bool _stopped;
} Clock;

Clock new_clock(void);

void stop_clock(Clock* clock);
void start_clock(Clock* clock);
void set_clock_run(Clock* clock, bool running);

Time get_elapsed_time(Clock* clock);

#endif
