#include <execinfo.h>

#include "engine_lib.h"
#include "engine.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_ENGINE
#include "logger.h"

uintptr_t impl_engine_crash(const char* file, int line, ShutdownReason reason) {
    log_error("Engine requested to crash! Shutting down rapidly and safely. [File: %s:%d]", file, line);

    const uintptr_t stacktrace_symbol_count = 64;
    void* stacktrace = malloc(stacktrace_symbol_count);
    int symbol_count = backtrace(stacktrace, stacktrace_symbol_count);
    char** symbols = backtrace_symbols(stacktrace, stacktrace_symbol_count);
    log_info_lines("Stack Trace");
    for (int i = 0; i < symbol_count; i++) {
        log_line("%s", symbols[i]);
    }
    log_line_commit();
    free(symbols);
    free(stacktrace);

    ENGINE->shutdown_reason = reason;
    engine_stop();
    abort();
    return 1;
}
