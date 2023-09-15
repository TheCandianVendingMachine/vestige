#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>

typedef enum LogLevel {
    DEBUG = 1 << 3,
    INFO =  1 << 2,
    WARN =  1 << 1,
    ERROR = 1 << 0,
} LogLevel;

static const char* LOG_LEVEL_STR[] = {
    "[ERROR]: ",
    "[WARN]: ",
    "[INFO]: ",
    "[DEBUG]: ",
};

typedef struct Logger {
    LogLevel levels;
} Logger;

extern Logger* LOGGER;

void log_debug(const char* format, ...);
void log_info(const char* format, ...);
void log_warning(const char* format, ...);
void log_error(const char* format, ...);
void log_level(LogLevel level, const char* format, ...);
void logger_start(void);
void logger_stop(void);

#endif
