#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "render.h"
#include "engine.h"
#include "logger.h"

void initialise_renderer(void) {
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        engine_crash(SHUTDOWN_CANT_INIT_GLAD);
    }

    log_info("GLAD loaded");
}
