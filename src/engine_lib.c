#include "engine_lib.h"
#include "engine.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_ENGINE
#include "logger.h"

uintptr_t impl_engine_crash(const char* file, int line, ShutdownReason reason) {
    log_error("Engine crashed! Shutting down... [File: %s:%d]", file, line);
    ENGINE->shutdown_reason = reason;
    engine_stop();
    abort();
    return 1;
}
