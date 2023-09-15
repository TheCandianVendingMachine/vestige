#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "render/shader.h"
#include "logger.h"

bool load_shader(Shader* shader, ShaderType type, const char* path) {
    shader->loaded = false;
    shader->compiled = false;

    FILE *file = fopen(path, "r");
    if (!file) {
        return false;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(length);
    const int buffer_length = fread(buffer, 1, length, file);

    fclose(file);

    shader->loaded = true;
    shader->_type = type;
    shader->_source = glCreateShader(type);

    const char* const buffers[] = { buffer };
    glShaderSource(shader->_source, 1, buffers, &buffer_length);

    return true;
}

bool load_vertex_shader(Shader* shader, const char* path) {
    return load_shader(shader, VERTEX_SHADER, path);
}

bool load_fragment_shader(Shader* shader, const char* path) {
    return load_shader(shader, FRAGMENT_SHADER, path);
}

ShaderProgram create_shader_program(Shader vertex_shader, Shader fragment_shader) {
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

    return program;
}

bool compile_shader(Shader* shader, GLsizei message_length, char** message) {
    if (!shader->loaded) {
        strcpy(*message, "Shader not loaded from disk, cannot compile");
        return false;
    }
    glCompileShader(shader->_source);

    GLint status;
    glGetShaderiv(shader->_source, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(shader->_source, message_length, NULL, *message);
        return false;
    }

    shader->compiled = true;
    return true;
}
