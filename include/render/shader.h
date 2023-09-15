#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <stdint.h>

#include <glad/glad.h>

typedef enum ShaderType {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER
} ShaderType;

typedef struct Shader {
    GLuint _source;
    ShaderType _type;
    bool loaded;
    bool compiled;
} Shader;

typedef struct ShaderProgram {
    Shader _vertex_shader;
    Shader _fragment_shader;
    GLuint _program;
} ShaderProgram;

ShaderProgram create_shader_program(Shader vertex_shader, Shader fragment_shader);

bool load_vertex_shader(Shader* shader, const char* path);
bool load_fragment_shader(Shader* shader, const char* path);

bool compile_shader(Shader* shader, GLsizei message_length, char** message);

#endif
