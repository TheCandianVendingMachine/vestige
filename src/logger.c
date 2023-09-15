#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define LOG_WITH_LEVEL(level, format) \
    va_list args; \
    va_start(args, format); \
    log_impl(level, format, args); \
    va_end(args)

Logger* LOGGER = NULL;

void log_impl(LogLevel level, const char* format, va_list args) {
    if ((level & LOGGER->levels) == 0) {
        return;
    }

    int level_index = 0;
    for (; ((level >> level_index) & 1) == 0; level_index++) { }

    printf("%s", LOG_LEVEL_STR[level_index]);
    vprintf(format, args);
    printf("\n");
}

void log_debug(const char* format, ...) {
    LOG_WITH_LEVEL(DEBUG, format);
}

void log_info(const char* format, ...) {
    LOG_WITH_LEVEL(INFO, format);
}

void log_warning(const char* format, ...) {
    LOG_WITH_LEVEL(WARN, format);
}

void log_error(const char* format, ...) {
    LOG_WITH_LEVEL(ERROR, format);
}

void log_level(LogLevel level, const char* format, ...) {
    LOG_WITH_LEVEL(level, format);
}

void logger_start(void) {
    LOGGER = malloc(sizeof(Logger));
    LOGGER->levels = DEBUG | INFO | WARN | ERROR;
    log_info("Logger Started");
}

void logger_stop(void) {
    free(LOGGER);
    LOGGER = NULL;
}
