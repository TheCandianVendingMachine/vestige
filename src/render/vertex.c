#include <glad/glad.h>

#include "render/vertex.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_RENDERER
#include "logger.h"

void bind_vertex_attributes(void) {
    int total_size = sizeof(Vertex);
    long int offset = 0;

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_size, (void*)offset);
    offset += sizeof(VertexPosition);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, total_size, (void*)offset);
    offset += sizeof(Vector2f);
    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, total_size, (void*)offset);
    offset += sizeof(Vector3f);
    // Color
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, total_size, (void*)offset);
    offset += sizeof(VertexColor);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}
