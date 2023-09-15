#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "render/shader.h"
#include "logger.h"

ShaderResult compile_shader(Shader shader) {
    glCompileShader(shader._source);

    GLint status;
    glGetShaderiv(shader._source, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        ShaderResult result;
        result.result = RESULT_ERROR;
        result.data.error = new_string(1024);
        glGetShaderInfoLog(shader._source, 1024, NULL, (char*)result.data.error.buffer);
        return result;
    }

    shader.compiled = true;
    return OK_RESULT(Shader, shader);
}

ShaderResult load_shader(ShaderType type, const char* path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        return ERR_RESULT(Shader, "Cannot open shader file");
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(length);
    const int buffer_length = fread(buffer, 1, length, file);

    fclose(file);

    Shader shader;
    shader.compiled = false;
    shader._type = type;
    shader._source = glCreateShader(type);

    const char* const buffers[] = { buffer };
    glShaderSource(shader._source, 1, buffers, &buffer_length);

    return compile_shader(shader);
}

ShaderResult load_vertex_shader(const char* path) {
    return load_shader(VERTEX_SHADER, path);
}

ShaderResult load_fragment_shader(const char* path) {
    return load_shader(FRAGMENT_SHADER, path);
}

ShaderProgramResult create_shader_program(Shader vertex_shader, Shader fragment_shader) {
    ShaderProgram program;
    program._vertex_shader = vertex_shader;
    program._fragment_shader = fragment_shader;
    program._program = glCreateProgram();

    if (program._vertex_shader.compiled) {
        glAttachShader(program._program, program._vertex_shader._source);
    }

    if (program._fragment_shader.compiled) {
        glAttachShader(program._program, program._fragment_shader._source);
    }

    glLinkProgram(program._program);

    GLint link_status;
    glGetProgramiv(program._program, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        ShaderProgramResult result;
        result.result = RESULT_ERROR;
        result.data.error = new_string(1024);
        glGetProgramInfoLog(program._program, 1024, NULL, (char*)result.data.error.buffer);
        return result;
    }

    return OK_RESULT(ShaderProgram, program);
}

