#ifndef CRASH_REASONS_H
#define CRASH_REASONS_H

typedef enum {
    SHUTDOWN_NORMAL = 0
} ShutdownReason;

static const char* SHUTDOWN_REASONS_STR[] = {
    "Normal Shutdown"
};

#endif
