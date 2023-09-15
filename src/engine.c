#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "engine.h"
#include "logger.h"

Engine* ENGINE = NULL;

void engine_start(void) {
    logger_start();

    ENGINE = malloc(sizeof(Engine));
    ENGINE->shutdown_reason = SHUTDOWN_NORMAL;

    log_info(ENGINE_NAME " started");
}

void engine_crash(ShutdownReason reason) {
    ENGINE->shutdown_reason = reason;
}

int engine_stop(void) {
    ShutdownReason shutdown_return = ENGINE->shutdown_reason;
    free(ENGINE);
    ENGINE = NULL;

    if (shutdown_return != SHUTDOWN_NORMAL) {
        log_warning(ENGINE_NAME " shutdown. Reason: %s", SHUTDOWN_REASONS_STR[shutdown_return]);
    } else {
        log_info(ENGINE_NAME " shutdown. Reason: %s", SHUTDOWN_REASONS_STR[shutdown_return]);
    }
    logger_stop();
    return shutdown_return;
}
