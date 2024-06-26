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

#define GET_MESSAGE_FROM_FORMAT(instance, indent, message, format)\
    va_list args; \
    va_start(args, format); \
    message = get_message_from_format((instance).channel, (instance).level, indent, format, args); \
    va_end(args)

Logger* LOGGER = NULL;

size_t get_index_from_bits(uint64_t bits) {
    unsigned long long index = 0;
    for (; ((bits >> index) & 1) == 0; index++) { }
    return index;
}

struct LogMessageLength log_to_file(
    FILE* file,
    unsigned long long channel_index,
    unsigned long long level_index,
    unsigned int indent_length,
    const char* format,
    va_list args
) {
    struct LogMessageLength length;

    if (LOGGER->log_colour) {
        length.zero_width_prefix = fprintf(file, "\033[0;%s", STDOUT_LEVEL_COLOURS[level_index]);
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
    int64_t bytes_written_indent = 0;
    for (; bytes_written_indent < indent_length; bytes_written_indent++) { fprintf(file, " "); }
    int64_t bytes_written_user = vfprintf(file, format, args);
    if (LOGGER->log_colour) {
        length.zero_width_postfix = fprintf(file, "\033[0;30m");
    }
    int64_t bytes_written_newline = fprintf(file, "\n");
    length.prefix = bytes_written_time + bytes_written_channel + bytes_written_level;
    length.total = bytes_written_indent + bytes_written_user + bytes_written_newline;
    length.total += length.prefix + length.zero_width_prefix + length.zero_width_postfix;
    return length;
}

void log_instance_to_file(FILE* file, struct LogInstance instance) {
    fwrite(instance.intro_message.message, 1, instance.intro_message.length.total, file);
    for (size_t i = 0; i < instance.body_message_count; i++) {
        struct LogMessage message = instance.body_messages[i];
        fwrite(message.message, 1, message.length.total, file);
    }
    fflush(file);
}

struct LogMessage get_message_from_format(
    LogChannel channel,
    LogLevel level,
    unsigned int indent,
    const char* format,
    va_list args
) {
    const size_t TRUNCATED_SIZE = sizeof(TRUNCATED_LOG_MESSAGE);

    struct LogMessage message;
    message.log_number = LOGGER->_log_number;

    unsigned long long level_index = get_index_from_bits(level);
    unsigned long long channel_index = get_index_from_bits(channel);

    FILE* channel_stream = fmemopen(message.message, MAX_SINGLE_LOG_LENGTH, "w");
    message.length = log_to_file(channel_stream, channel_index, level_index, indent, format, args);
    fflush(channel_stream);
    fclose(channel_stream);

    if (message.length.total >= MAX_SINGLE_LOG_LENGTH) {
        // this macro includes the null byte, which isnt a character we count
        message.length.total = MAX_TRUNCATED_LOG_LENGTH - 1;
        memcpy(message.message + MAX_SINGLE_LOG_LENGTH - 1, TRUNCATED_LOG_MESSAGE, TRUNCATED_SIZE);
    }

    return message;
}

void log_instance_to_channel(
    struct LogInstance instance
) {
    if ((instance.level & LOGGER->levels) == 0) {
        return;
    }

    if (instance.verbosity > LOGGER->allowed_verbosity) {
        return;
    }

    LOGGER->_log_number += 1;

    unsigned long long channel_index = get_index_from_bits(instance.channel);

    if ((instance.channel & LOGGER->suppressed_channels_stdout) == 0) {
        log_instance_to_file(stdout, instance);
    }

    struct LogHistory* channel_history = &LOGGER->channel_history[channel_index];
    channel_history->history[channel_history->write_index] = instance;
    channel_history->write_index = (channel_history->write_index + 1) % MAX_LOG_HISTORY;
    channel_history->length++;
    channel_history->length = channel_history->length > MAX_LOG_HISTORY ? MAX_LOG_HISTORY : channel_history->length;
}

struct LogInstance create_empty_instance(LogChannel channel, LogLevel level, unsigned int verbosity) {
    return (struct LogInstance) {
        .verbosity = verbosity,
        .level = level,
        .channel = channel,
        .body_message_count = 0
    };
}

struct LogInstance log_info_instance_intro(LogChannel channel, unsigned int verbosity, const char* format, ...) {
    struct LogInstance instance = create_empty_instance(channel, INFO, verbosity);
    GET_MESSAGE_FROM_FORMAT(instance, 0, instance.intro_message, format);
    return instance;
}

struct LogInstance log_warning_instance_intro(LogChannel channel, unsigned int verbosity, const char* format, ...) {
    struct LogInstance instance = create_empty_instance(channel, WARN, verbosity);
    GET_MESSAGE_FROM_FORMAT(instance, 0, instance.intro_message, format);
    return instance;
}

struct LogInstance log_error_instance_intro(LogChannel channel, unsigned int verbosity, const char* format, ...) {
    struct LogInstance instance = create_empty_instance(channel, ERROR, verbosity);
    GET_MESSAGE_FROM_FORMAT(instance, 0, instance.intro_message, format);
    return instance;
}

struct LogInstance log_debug_instance_intro(LogChannel channel, unsigned int verbosity, const char* format, ...) {
    struct LogInstance instance = create_empty_instance(channel, DEBUG, verbosity);
    GET_MESSAGE_FROM_FORMAT(instance, 0, instance.intro_message, format);
    return instance;
}

struct LogInstance log_level_instance_intro(
    LogChannel channel,
    LogLevel level,
    unsigned int verbosity,
    const char* format, ...
) {
    struct LogInstance instance = create_empty_instance(channel, level, verbosity);
    GET_MESSAGE_FROM_FORMAT(instance, 0, instance.intro_message, format);
    return instance;
}

void log_line_to_instance(struct LogInstance* instance, unsigned int verbosity, const char* format, ...) {
    if (instance->body_message_count > MAX_LOG_LINES) { return; }
    if (instance->body_message_count == MAX_LOG_LINES) {
        va_list args;
        va_start(args, format);
        instance->body_messages[instance->body_message_count] = get_message_from_format(
            instance->channel,
            instance->level,
            0,
            "<subsequent lines truncated>",
            args
        );
        va_end(args);
    } else {
        GET_MESSAGE_FROM_FORMAT(
            *instance,
            LOGGER->indent_length,
            instance->body_messages[instance->body_message_count],
            format
        );
        instance->body_messages[instance->body_message_count].verbosity = verbosity;
    }
    struct LogMessage* message = &instance->body_messages[instance->body_message_count];
    memset(message->message + message->length.zero_width_prefix, '-', message->length.prefix - 1);
    instance->body_message_count += 1;
}

void log_instance_commit(struct LogInstance instance) {
    log_instance_to_channel(instance);
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
            struct LogInstance instance = history->history[(history->write_index + i) % length];
            log_instance_to_file(out_file, instance);
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
    LOGGER->indent_length = 2;

    for (int i = 0; i < LOG_CHANNEL_COUNT; i++) {
        struct LogHistory history = {
            .history = malloc(sizeof(struct LogInstance) * MAX_LOG_HISTORY),
            .length = 0,
            .write_index = 0
        };
        LOGGER->channel_history[i] = history;
    }

    log_instance_commit(log_info_instance_intro(LOG_CHANNEL_CORE, INFO, "Logger Started"));
}

void logger_stop(void) {
    free(LOGGER);
    LOGGER = NULL;
}
