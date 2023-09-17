#include <glad/glad.h>

#include "render/vertex.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_RENDERER
#include "logger.h"

void bind_vertex_attributes(void) {
    int total_size = sizeof(VertexPosition) + sizeof(VertexColor);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_size, (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, total_size, (void*)sizeof(VertexPosition));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    log_debug("Bound Vertex attributes: [%d %d]", sizeof(VertexPosition), sizeof(VertexColor));
}
