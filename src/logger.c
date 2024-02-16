// For fmemopen
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "logger.h"
#include "lib/time.h"

const char* STDOUT_LEVEL_COLOURS[4] = {
    "31m", // Error
    "35m", // Warn
    "37m", // Info
    "33m"  // Debug
};

#define LOG_WITH_LEVEL(channel, level, verbosity, format) \
    va_list args; \
    va_start(args, format); \
    log_impl_to_channel(channel, level, verbosity, format, args); \
    va_end(args)

Logger* LOGGER = NULL;

int64_t log_to_file(
    FILE* file,
    unsigned long long channel_index,
    unsigned long long level_index,
    const char* format,
    va_list args
) {
    if (LOGGER->log_colour && file == stdout) {
        fprintf(file, "\033[0;%s", STDOUT_LEVEL_COLOURS[level_index]);
    }

    int64_t bytes_written_time = 0;
    if (LOGGER->log_time) {
        Time elapsed = get_elapsed_time(&LOGGER->clock);
        float seconds = time_as_seconds(elapsed);
        unsigned int milliseconds = floorf((seconds - floorf(seconds)) * 1000.f);
        unsigned int minutes = floorf(seconds / 60.f);
        unsigned int hours = floorf(minutes / 60.f);
        bytes_written_time = fprintf(file,
                "%.2d:%.2d:%.2d.%.3d",
                hours,
                minutes % 60,
                (int)seconds % 60,
                milliseconds
        );
    }
    int64_t bytes_written_channel = fprintf(file, "%s", LOG_CHANNEL_STR[channel_index]);
    int64_t bytes_written_level = fprintf(file, "%s", LOG_LEVEL_STR[level_index]);
    int64_t bytes_written_user = vfprintf(file, format, args);
    int64_t bytes_written_newline = 0;
    if (file == stdout) {
        if (LOGGER->log_colour) {
            fprintf(file, "\033[0;30m");
        }
        bytes_written_newline = fprintf(file, "\n");
    }
    return  bytes_written_time +
            bytes_written_channel +
            bytes_written_level +
            bytes_written_user +
            bytes_written_newline;
}

void log_impl_to_channel(
    LogChannel channel,
    LogLevel level,
    unsigned int verbosity,
    const char* format,
    va_list args
) {
    if ((level & LOGGER->levels) == 0) {
        return;
    }

    if (verbosity > LOGGER->allowed_verbosity) {
        return;
    }

    LOGGER->_log_number += 1;

    unsigned long long level_index = 0;
    for (; ((level >> level_index) & 1) == 0; level_index++) { }

    unsigned long long channel_index = 0;
    for (; ((channel >> channel_index) & 1) == 0; channel_index++) { }

    if ((channel & LOGGER->suppressed_channels_stdout) == 0) {
        va_list stdout_args;
        va_copy(stdout_args, args);
        log_to_file(stdout, channel_index, level_index, format, stdout_args);
    }

    const size_t TRUNCATED_SIZE = sizeof(TRUNCATED_LOG_MESSAGE);

    struct LogMessage message;
    message.log_number = LOGGER->_log_number;

    FILE* channel_stream = fmemopen(message.message, MAX_SINGLE_LOG_LENGTH, "w");
    message.length = log_to_file(channel_stream, channel_index, level_index, format, args);
    fflush(channel_stream);
    fclose(channel_stream);

    if (message.length >= MAX_SINGLE_LOG_LENGTH) {
        // this macro includes the null byte, which isnt a character we count
        message.length = MAX_TRUNCATED_LOG_LENGTH - 1;
        memcpy(message.message + MAX_SINGLE_LOG_LENGTH - 1, TRUNCATED_LOG_MESSAGE, TRUNCATED_SIZE);
    }

    struct LogHistory* channel_history = &LOGGER->channel_history[channel_index];
    channel_history->history[channel_history->write_index] = message;
    channel_history->write_index = (channel_history->write_index + 1) % MAX_LOG_HISTORY;
    channel_history->length++;
    channel_history->length = channel_history->length > MAX_LOG_HISTORY ? MAX_LOG_HISTORY : channel_history->length;
}

void log_debug_to_channel(LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, DEBUG, 0, format);
}

void log_debug_to_channel_verbose(unsigned int verbosity, LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, DEBUG, verbosity, format);
}

void log_info_to_channel(LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, INFO, 0, format);
}

void log_warning_to_channel(LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, WARN, 0, format);
}

void log_error_to_channel(LogChannel channel, const char* format, ...) {
    LOG_WITH_LEVEL(channel, ERROR, 0, format);
}

void log_level_to_channel(LogChannel channel, LogLevel level, const char* format, ...) {
    LOG_WITH_LEVEL(channel, level, 0, format);
}

void logger_dump_channel_to_disk(const char* output_path, LogChannel channel) {
    for (unsigned long long i = 0; i < LOG_CHANNEL_COUNT; i++) {
        if (((channel >> i) & 1) == 0) { continue; }

        struct LogHistory* history = &LOGGER->channel_history[i];

        char* path = malloc(strlen(output_path) + strlen(LOG_CHANNEL_OUTPUT[i]) + 1);
        sprintf(path, "%s/%s", output_path, LOG_CHANNEL_OUTPUT[i]);
        FILE* out_file = fopen(path, "a");

        const size_t length = MAX_LOG_HISTORY > history->length ? history->length : MAX_LOG_HISTORY;
        for (int i = 0; i < history->length; i++) {
            struct LogMessage* message = &history->history[(history->write_index + i) % length];
            fwrite(message->message, 1, message->length, out_file);
            fprintf(out_file, "\n");
        }

        fflush(out_file);
        fclose(out_file);
    }
}

void logger_start(void) {
    LOGGER = malloc(sizeof(Logger));
    LOGGER->_log_number = 0;
    LOGGER->levels = DEBUG | INFO | WARN | ERROR;
    LOGGER->allowed_verbosity = 0;
    LOGGER->suppressed_channels_stdout = LOG_CHANNEL_NONE;
    LOGGER->clock = new_clock();
    LOGGER->log_time = true;
    LOGGER->log_colour = true;

    for (int i = 0; i < LOG_CHANNEL_COUNT; i++) {
        struct LogHistory history = {
            .history = malloc(sizeof(struct LogMessage) * MAX_LOG_HISTORY),
            .length = 0,
            .write_index = 0
        };
        LOGGER->channel_history[i] = history;
    }

    log_info_to_channel(LOG_CHANNEL_CORE, "Logger Started");
}

void logger_stop(void) {
    free(LOGGER);
    LOGGER = NULL;
}
