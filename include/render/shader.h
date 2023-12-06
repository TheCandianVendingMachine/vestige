#ifndef VESTIGE_SHADER_H
#define VESTIGE_SHADER_H

#include <stdbool.h>
#include <stdint.h>

#include "glad/glad.h"
#include "lib/result.h"
#include "lib/filesystem.h"

typedef enum ShaderType {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER
} ShaderType;

typedef struct Shader {
    FileMetaData file;
    GLuint _source;
    ShaderType _type;
} Shader;

typedef struct ShaderProgram {
    Shader _vertex_shader;
    Shader _fragment_shader;
    GLuint _program;
} ShaderProgram;

DEFINE_RESULT(Shader);
DEFINE_RESULT(ShaderProgram);

ShaderResult load_vertex_shader_from_buffer(GLint buffer_length, const char* buffer);
ShaderResult load_fragment_shader_from_buffer(GLint buffer_length, const char* buffer);

ShaderResult load_vertex_shader_from_disk(const char* path);
ShaderResult load_fragment_shader_from_disk(const char* path);

ShaderProgramResult create_shader_program(Shader vertex_shader, Shader fragment_shader);
void update_shader_program(ShaderProgram* program);

#endif
