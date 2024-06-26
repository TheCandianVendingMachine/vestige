#ifndef VESTIGE_LOG_CHANNEL
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GENERIC
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define log_info_lines(format, ...)\
    do { struct LogInstance instance = log_info_instance_intro((VESTIGE_LOG_CHANNEL), 0, (format), ##__VA_ARGS__)
#define log_warning_lines(format, ...)\
    do { struct LogInstance instance = log_warning_instance_intro((VESTIGE_LOG_CHANNEL), 0, (format), ##__VA_ARGS__)
#define log_error_lines(format, ...)\
    do { struct LogInstance instance = log_error_instance_intro((VESTIGE_LOG_CHANNEL), 0, (format), ##__VA_ARGS__)
#define log_debug_lines(format, ...)\
    do { struct LogInstance instance = log_debug_instance_intro((VESTIGE_LOG_CHANNEL), 0, (format), ##__VA_ARGS__)
#define log_debug_lines_verbose(verbosity, format, ...)\
    do { struct LogInstance instance = log_debug_instance_intro((VESTIGE_LOG_CHANNEL), (verbosity), (format), ##__VA_ARGS__)

#define log_line(format, ...)                       log_line_to_instance(&instance, 0, (format), ##__VA_ARGS__)
#define log_line_verbose(verbosity, format, ...)    log_line_to_instance(&instance, (verbosity), (format), ##__VA_ARGS__)
#define log_line_commit()                           log_instance_commit(instance); } while (0)

#define log_info(format, ...)                       log_info_lines((format), ##__VA_ARGS__); log_line_commit()
#define log_warning(format, ...)                    log_warning_lines((format), ##__VA_ARGS__); log_line_commit()
#define log_error(format, ...)                      log_error_lines((format), ##__VA_ARGS__); log_line_commit()
#define log_debug(format, ...)                      log_debug_lines((format), ##__VA_ARGS__); log_line_commit()
#define log_debug_verbose(verbosity, format, ...)\
    log_debug_lines_verbose((verbosity), (format), ##__VA_ARGS__); log_line_commit()

#pragma clang diagnostic pop


#ifndef LOGGER_H
#define LOGGER_H

#define TRUNCATED_LOG_MESSAGE "... <TRUNCATED>"
#define MAX_SINGLE_LOG_LENGTH 512
#define MAX_TRUNCATED_LOG_LENGTH (MAX_SINGLE_LOG_LENGTH + sizeof(TRUNCATED_LOG_MESSAGE))
#define MAX_LOG_HISTORY 1024
#define MAX_LOG_LINES 24

#include <stdio.h>
#include <stdbool.h>

#include "lib/clock.h"

typedef enum LogLevel {
    DEBUG = 1 << 3,
    INFO =  1 << 2,
    WARN =  1 << 1,
    ERROR = 1 << 0,
    LOG_LEVEL_COUNT = 4
} LogLevel;

typedef enum LogChannel {
    LOG_CHANNEL_NONE        = 0,
    LOG_CHANNEL_ALL         = ~0,
    LOG_CHANNEL_GENERIC     = 1 << 0,
    LOG_CHANNEL_CORE        = 1 << 1,
    LOG_CHANNEL_ENGINE      = 1 << 2,
    LOG_CHANNEL_RENDERER    = 1 << 3,
    LOG_CHANNEL_PHYSICS     = 1 << 4,
    LOG_CHANNEL_AUDIO       = 1 << 5,
    LOG_CHANNEL_UI          = 1 << 6,
    LOG_CHANNEL_INPUT       = 1 << 7,
    LOG_CHANNEL_GAME        = 1 << 8,
    LOG_CHANNEL_COUNT       = 9
} LogChannel;

static const char* LOG_LEVEL_STR[LOG_LEVEL_COUNT] = {
    "[ERROR]: ",
    "[WARN]: ",
    "[INFO]: ",
    "[DEBUG]: ",
};

static const char* LOG_CHANNEL_STR[LOG_CHANNEL_COUNT] = {
    "(GENERIC)",
    "(CORE)",
    "(ENGINE)",
    "(RENDERER)",
    "(PHYSICS)",
    "(AUDIO)",
    "(UI)",
    "(INPUT)",
    "(GAME)",
};

static const char* LOG_CHANNEL_OUTPUT[LOG_CHANNEL_COUNT] = {
    "generic.log",
    "core.log",
    "engine.log",
    "renderer.log",
    "physics.log",
    "audio.log",
    "ui.log",
    "input.log",
    "game.log",
};

struct LogMessageLength {
    int64_t total;
    int64_t zero_width_prefix;
    int64_t zero_width_postfix;
    int64_t prefix;
};

struct LogMessage {
    uint64_t log_number;
    unsigned int verbosity;
    struct LogMessageLength length;
    char message[MAX_TRUNCATED_LOG_LENGTH];
};

struct LogInstance {
    LogChannel channel;
    LogLevel level;
    unsigned int verbosity;
    size_t body_message_count;
    struct LogMessage intro_message;
    struct LogMessage body_messages[MAX_LOG_LINES + 1];
};

struct LogHistory {
    struct LogInstance* history;
    size_t length;
    size_t write_index;
};

typedef struct Logger {
    uint64_t _log_number;
    LogLevel levels;
    unsigned int indent_length;
    unsigned int allowed_verbosity;
    LogChannel suppressed_channels_stdout;
    struct LogHistory channel_history[LOG_CHANNEL_COUNT];
    Clock clock;
    bool log_time;
    bool log_colour;
} Logger;

extern Logger* LOGGER;

struct LogInstance log_info_instance_intro(LogChannel channel, unsigned int verbosity, const char* format, ...);
struct LogInstance log_warning_instance_intro(LogChannel channel, unsigned int verbosity, const char* format, ...);
struct LogInstance log_error_instance_intro(LogChannel channel, unsigned int verbosity, const char* format, ...);
struct LogInstance log_debug_instance_intro(LogChannel channel, unsigned int verbosity, const char* format, ...);
struct LogInstance log_level_instance_intro(LogChannel channel, LogLevel level, unsigned int verbosity, const char* format, ...);
void log_line_to_instance(struct LogInstance* instance, unsigned int verbosity, const char* format, ...);
void log_instance_commit(struct LogInstance instance);

void logger_dump_channel_to_disk(const char* output_path, LogChannel channel);
void logger_start(void);
void logger_stop(void);

#endif
