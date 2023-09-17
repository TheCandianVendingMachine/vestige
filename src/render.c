#include "glad/glad.h"

#include "render.h"
#include "engine.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_RENDERER
#include "logger.h"

void initialise_renderer(void) {
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        engine_crash(SHUTDOWN_CANT_INIT_GLAD);
    }

    log_info("GLAD loaded");
}
