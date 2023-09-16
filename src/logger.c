#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define LOG_WITH_LEVEL(channel, level, format) \
    va_list args; \
    va_start(args, format); \
    log_impl_to_channel(channel, level, format, args); \
    va_end(args)

Logger* LOGGER = NULL;

void log_impl_to_channel(LogChannel channel, LogLevel level, const char* format, va_list args) {
    if ((level & LOGGER->levels) == 0) {
        return;
    }

    int level_index = 0;
    for (; ((level >> level_index) & 1) == 0; level_index++) { }

    unsigned long long channel_index = 0;
    for (; ((channel >> channel_index) & 1) == 0; channel_index++) { }

    printf("%s", LOG_CHANNEL_STR[channel_index]);
    printf("%s", LOG_LEVEL_STR[level_index]);
    vprintf(format, args);
    printf("\n");
}

void log_debug_to_channel(LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, DEBUG, format);
}

void log_info_to_channel(LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, INFO, format);
}

void log_warning_to_channel(LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, WARN, format);
}

void log_error_to_channel(LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, ERROR, format);
}

void log_level_to_channel(LogChannel channel, LogLevel level, const char* format, ...) {
    LOG_WITH_LEVEL(channel, level, format);
}

void logger_start(void) {
    LOGGER = malloc(sizeof(Logger));
    LOGGER->levels = DEBUG | INFO | WARN | ERROR;
    log_info_to_channel(LOG_CHANNEL_CORE, "Logger Started");
}

void logger_stop(void) {
    free(LOGGER);
    LOGGER = NULL;
}
