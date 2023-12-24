#ifndef VESTIGE_LOG_CHANNEL
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GENERIC
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define log_info(format, ...)       log_info_to_channel((VESTIGE_LOG_CHANNEL), (format), ##__VA_ARGS__)
#define log_warning(format, ...)    log_warning_to_channel((VESTIGE_LOG_CHANNEL), (format), ##__VA_ARGS__)
#define log_error(format, ...)      log_error_to_channel((VESTIGE_LOG_CHANNEL), (format), ##__VA_ARGS__)
#define log_debug(format, ...)      log_debug_to_channel((VESTIGE_LOG_CHANNEL), (format), ##__VA_ARGS__)
#define log_debug_verbose(verbosity, format, ...)\
                                    log_debug_to_channel_verbose((verbosity), (VESTIGE_LOG_CHANNEL), (format), ##__VA_ARGS__)

#pragma clang diagnostic pop


#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <stdbool.h>

#include "lib/clock.h"

typedef enum LogLevel {
    DEBUG = 1 << 3,
    INFO =  1 << 2,
    WARN =  1 << 1,
    ERROR = 1 << 0,
} LogLevel;

typedef enum LogChannel {
    LOG_CHANNEL_NONE        = 0,
    LOG_CHANNEL_ALL         = ~0,
    LOG_CHANNEL_GENERIC     = 1 << 0,
    LOG_CHANNEL_CORE        = 1 << 1,
    LOG_CHANNEL_ENGINE      = 1 << 2,
    LOG_CHANNEL_RENDERER    = 1 << 3,
    LOG_CHANNEL_AUDIO       = 1 << 4,
    LOG_CHANNEL_UI          = 1 << 5,
    LOG_CHANNEL_GAME        = 1 << 6
} LogChannel;

static const char* LOG_LEVEL_STR[] = {
    "[ERROR]: ",
    "[WARN]: ",
    "[INFO]: ",
    "[DEBUG]: ",
};

static const char* LOG_CHANNEL_STR[] = {
    "(GENERIC)",
    "(CORE)",
    "(ENGINE)",
    "(RENDERER)",
    "(AUDIO)",
    "(UI)",
    "(GAME)"
};

typedef struct Logger {
    LogLevel levels;
    unsigned int allowed_verbosity;
    LogChannel suppressed_channels_stdout;
    Clock clock;
    bool log_time;
} Logger;

extern Logger* LOGGER;

void log_debug_to_channel(LogChannel channel, const char* format, ...);
void log_debug_to_channel_verbose(unsigned int verbosity, LogChannel channel, const char* format, ...);
void log_info_to_channel(LogChannel channel, const char* format, ...);
void log_warning_to_channel(LogChannel channel, const char* format, ...);
void log_error_to_channel(LogChannel channel, const char* format, ...);
void log_level_to_channel(LogChannel channel, LogLevel level, const char* format, ...);
void logger_start(void);
void logger_stop(void);

#endif
