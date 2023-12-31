#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "render/shader.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_RENDERER
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
        log_warning("Could not compile shader:\n%s", result.data.error.buffer);
        return result;
    }

    return OK_RESULT(Shader, shader);
}

ShaderResult load_shader(ShaderType type, GLint buffer_length, const char* buffer) {
    Shader shader;
    shader._type = type;
    shader._source = glCreateShader(type);

    const char* const buffers[] = { buffer };
    glShaderSource(shader._source, 1, buffers, &buffer_length);

    return compile_shader(shader);
}

ShaderResult load_vertex_shader_from_buffer(GLint buffer_length, const char* buffer) {
    return load_shader(VERTEX_SHADER, buffer_length, buffer);
}

ShaderResult load_fragment_shader_from_buffer(GLint buffer_length, const char* buffer) {
    return load_shader(FRAGMENT_SHADER, buffer_length, buffer);
}

ShaderResult load_vertex_shader_from_disk(const char* path) {
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

    ShaderResult result = load_vertex_shader_from_buffer(buffer_length, buffer);
    free(buffer);

    if (result.result == RESULT_OK) {
        result.data.ok.file = file_meta_data(path);
    }
    return result;
}

ShaderResult load_fragment_shader_from_disk(const char* path) {
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

    ShaderResult result = load_fragment_shader_from_buffer(buffer_length, buffer);
    free(buffer);
    if (result.result == RESULT_OK) {
        result.data.ok.file = file_meta_data(path);
    }
    return result;
}

ShaderProgramResult create_shader_program(Shader vertex_shader, Shader fragment_shader) {
    ShaderProgram program;
    program._vertex_shader = vertex_shader;
    program._fragment_shader = fragment_shader;
    program._program = glCreateProgram();

    glAttachShader(program._program, program._vertex_shader._source);
    glAttachShader(program._program, program._fragment_shader._source);

    glLinkProgram(program._program);

    GLint link_status;
    glGetProgramiv(program._program, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        ShaderProgramResult result;
        result.result = RESULT_ERROR;
        result.data.error = new_string(1024);
        glGetProgramInfoLog(program._program, 1024, NULL, (char*)result.data.error.buffer);
        log_warning("Could not link shader:\n%s", result.data.error.buffer);
        return result;
    }

    glDeleteShader(vertex_shader._source);
    glDeleteShader(fragment_shader._source);

    return OK_RESULT(ShaderProgram, program);
}

void update_shader_program(ShaderProgram* program) {
    bool has_updated = false;
    if (has_file_been_modified(&program->_vertex_shader.file)) {
        log_debug("Updating vertex shader for program [%d]", program->_program);
        FileMetaData metadata = program->_vertex_shader.file;
        const char* filepath = cstr_from_string(metadata.file_path);
        ShaderResult load_result = load_vertex_shader_from_disk(cstr_from_string(metadata.file_path));
        if (load_result.result == RESULT_ERROR) {
            const char* err_str = cstr_from_string(load_result.data.error);
            log_error("Cannot reload \"%s\": %s", filepath, err_str);
            free((char*)err_str);
        } else {
            program->_vertex_shader = load_result.data.ok;
            destroy_file_meta_data(&metadata);
            has_updated = true;
        }
        free((char*)filepath);
    }

    if (has_file_been_modified(&program->_fragment_shader.file)) {
        log_debug("Updating fragment shader for program [%d]", program->_program);
        FileMetaData metadata = program->_fragment_shader.file;
        const char* filepath = cstr_from_string(metadata.file_path);
        ShaderResult load_result = load_fragment_shader_from_disk(filepath);
        if (load_result.result == RESULT_ERROR) {
            const char* err_str = cstr_from_string(load_result.data.error);
            log_error("Cannot reload \"%s\": %s", filepath, err_str);
            free((char*)err_str);
        } else {
            program->_fragment_shader = load_result.data.ok;
            destroy_file_meta_data(&metadata);
            has_updated = true;
        }
        free((char*)filepath);
    }

    if (has_updated) {
        log_debug("Recreating program [%d]", program->_program);
        glDeleteProgram(program->_program);
        *program = create_shader_program(program->_vertex_shader, program->_fragment_shader).data.ok;
    }
}
