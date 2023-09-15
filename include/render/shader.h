#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>
#include <stdint.h>

#include <glad/glad.h>

#include "lib/result.h"

typedef enum ShaderType {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER
} ShaderType;

typedef struct Shader {
    GLuint _source;
    ShaderType _type;
    bool compiled;
} Shader;

typedef struct ShaderProgram {
    Shader _vertex_shader;
    Shader _fragment_shader;
    GLuint _program;
} ShaderProgram;

DEFINE_RESULT(Shader);
DEFINE_RESULT(ShaderProgram);

ShaderProgramResult create_shader_program(Shader vertex_shader, Shader fragment_shader);

ShaderResult load_vertex_shader(const char* path);
ShaderResult load_fragment_shader(const char* path);

#endif
