#include "lib/clock.h"

Clock new_clock(void) {
    return (Clock) {
        ._start = current_time(),
        ._stop = zero_time(),
        ._offset = zero_time(),
        ._stopped = false
    };
}

void stop_clock(Clock* clock) {
    if (clock->_stopped) {
        return;
    }
    clock->_stop = current_time();
    clock->_stopped = true;
}

void start_clock(Clock* clock) {
    if (!clock->_stopped) {
        return;
    }
    clock->_offset = time_add(clock->_offset, time_sub(clock->_stop, clock->_start));
    clock->_start = current_time();
    clock->_stop = zero_time();
    clock->_stopped = false;
}

void set_clock_run(Clock* clock, bool running) {
    if (running) {
        start_clock(clock);
    } else {
        stop_clock(clock);
    }
}

Time get_elapsed_time(Clock* clock) {
    if (clock->_stopped) {
        return time_add(clock->_offset, time_elapsed(clock->_start, clock->_stop));
    } else {
        return time_add(clock->_offset, time_elapsed(clock->_start, current_time()));
    }
}
