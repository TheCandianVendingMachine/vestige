#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "logger.h"
#include "lib/time.h"

#define LOG_WITH_LEVEL(channel, level, format) \
    va_list args; \
    va_start(args, format); \
    log_impl_to_channel(channel, level, format, args); \
    va_end(args)

Logger* LOGGER = NULL;

void log_to_file(
    FILE* file,
    unsigned long long channel_index,
    unsigned long long level_index,
    const char* format,
    va_list args
) {
    if (LOGGER->log_time) {
        Time elapsed = get_elapsed_time(&LOGGER->clock);
        float seconds = time_as_seconds(elapsed);
        unsigned int milliseconds = floorf((seconds - floorf(seconds)) * 1000.f);
        unsigned int minutes = floorf(seconds / 60.f);
        unsigned int hours = floorf(minutes / 60.f);
        fprintf(file,
                "%.2d:%.2d:%.2d.%.3d",
                hours,
                minutes % 60,
                (int)seconds % 60,
                milliseconds
        );
    }
    fprintf(file, "%s", LOG_CHANNEL_STR[channel_index]);
    fprintf(file, "%s", LOG_LEVEL_STR[level_index]);
    vfprintf(file, format, args);
    fprintf(file, "\n");
}

void log_impl_to_channel(
    LogChannel channel,
    LogLevel level,
    const char* format,
    va_list args
) {
    if ((level & LOGGER->levels) == 0) {
        return;
    }

    int level_index = 0;
    for (; ((level >> level_index) & 1) == 0; level_index++) { }

    unsigned long long channel_index = 0;
    for (; ((channel >> channel_index) & 1) == 0; channel_index++) { }

    if ((channel & LOGGER->suppressed_channels_stdout) == 0) {
        log_to_file(stdout, channel_index, level_index, format, args);
    }
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
    LOGGER->suppressed_channels_stdout = LOG_CHANNEL_NONE;
    LOGGER->clock = new_clock();
    LOGGER->log_time = true;
    log_info_to_channel(LOG_CHANNEL_CORE, "Logger Started");
}

void logger_stop(void) {
    free(LOGGER);
    LOGGER = NULL;
}
