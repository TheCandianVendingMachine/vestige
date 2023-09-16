#include "glad/glad.h"

#include "render.h"
#include "engine.h"
#include "logger.h"
#include "render/vertex.h"

void initialise_renderer(void) {
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        engine_crash(SHUTDOWN_CANT_INIT_GLAD);
    }

    log_info("GLAD loaded");
    bind_vertex_attributes();
}
