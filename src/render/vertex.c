#include <glad/glad.h>

#include "render/vertex.h"
#include "logger.h"

void bind_vertex_attributes(void) {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexColor), (void*)sizeof(VertexPosition));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    log_debug("Bound Vertex attributes");
}