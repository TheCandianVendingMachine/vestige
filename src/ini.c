#define DEFAULT_PAIR_COUNT 8
#define DEFAULT_PATH_LENGTH 4

#include <stdlib.h>
#include <string.h>

#include "ini.h"

void insert_into_section(IniSection* section, const char** section_path, const char* key, void* value, int value_type) {
    if (*section_path == NULL) {
        // path is empty, insert key into section
        if (section->pair_length >= section->pair_size) {
            section->pair_size *= 2;
            section->pairs = realloc(section->pairs, sizeof(struct IniPair) * section->pair_size);
        }
        section->pairs[section->pair_length].key = key;
        section->pairs[section->pair_length].type = value_type;
        section->pairs[section->pair_length].value = value;
        section->pair_length += 1;
        return;
    }

    const char* section_name = *section_path;
    IniSection* child = NULL;
    while (1) {
        int direction = strcmp(section_name, section->name);
        if (direction < 0) {
            if (section->children[0] == NULL) {
                section->children[0] = malloc(sizeof(IniSection));
                child = section->children[0];
                break;
            }
            section = section->children[0];
        } else {
            if (section->children[1] == NULL) {
                section->children[1] = malloc(sizeof(IniSection));
                child = section->children[1];
                break;
            }
            section = section->children[1];
        }
    }

    child->pairs = malloc(sizeof(struct IniPair) * DEFAULT_PAIR_COUNT);
    child->pair_length = 0;
    child->pair_size = DEFAULT_PAIR_COUNT;
    child->name = section_name;
    child->children[0] = NULL;
    child->children[1] = NULL;

    insert_into_section(child, section_path + 1, key, value, value_type);
}

const char** get_section_path_from_string(const char* path) {
    size_t key_length = strlen(path);

    char** section_path = malloc(sizeof(const char*) * DEFAULT_PATH_LENGTH);
    section_path[0] = NULL;
    size_t path_length = DEFAULT_PATH_LENGTH;
    size_t path_index = 0;

    char* built_section = malloc(key_length);
    size_t built_section_index = 0;

    char c;
    int j = 0;
    while ((c = path[j++])) {
        if (c == '.' || path[j] == '\0') {
            built_section[built_section_index] = '\0';
            section_path[path_index] = malloc(sizeof(char) * (1 + built_section_index));
            memcpy(section_path[path_index], built_section, built_section_index);
            section_path[path_index][built_section_index] = '\0';

            path_index += 1;
            if (path_index >= path_length) {
                path_length *= 2;
                section_path = realloc(section_path, sizeof(const char*) * path_length);
            }
            section_path[path_index] = NULL;
            built_section_index = 0;
        } else {
            built_section[built_section_index++] = c;
        }
    }

    free(built_section);
    return (const char**)section_path;
}

IniFile construct_ini_from_defaults(IniDefault defaults) {
    IniFile file;
    file.defaults = defaults;
    file.global_section.children[0] = NULL;
    file.global_section.children[1] = NULL;
    file.global_section.name = "";
    file.global_section.pairs = malloc(sizeof(IniSection) * DEFAULT_PAIR_COUNT);
    file.global_section.pair_length = 0;
    file.global_section.pair_size = DEFAULT_PAIR_COUNT;

    for (int i = 0; i < defaults.count; i++) {
        size_t key_length = strlen(defaults.pairs[i].key);
        size_t key_split = 0;
        for (int j = key_length - 1; j >= 0; j--) {
            if (defaults.pairs[i].key[j] == '.') {
                key_split = j;
                break;
            }
        }

        char* value_key = malloc(sizeof(char) * (1 + key_length - key_split));
        char* value_path = malloc(sizeof(char) * (key_split + 1));
        memset(value_key, '\0', sizeof(char) * (1 + key_length - key_split));
        memset(value_path, '\0', sizeof(char) * (key_split + 1));
        memcpy(value_key, defaults.pairs[i].key + key_split + 1, key_length - key_split);
        memcpy(value_path, defaults.pairs[i].key, key_split);

        const char** section_path = get_section_path_from_string(value_path);

        insert_into_section(
            &file.global_section,
            section_path,
            value_key, (void*)defaults.pairs[i].value.string,
            VALUE_TYPE_STRING
        );

        free(section_path);
        free(value_path);
    }

    return file;
}

void destroy_ini_section(IniSection* section) {
    for (int i = 0; i < section->pair_length; i++) {
        free((void*)section->pairs[i].key);
    }
    free(section->pairs);
    if (section->children[0] != NULL) {
        destroy_ini_section(section->children[0]);
        free((void*)section->children[0]->name);
        free(section->children[0]);
    }
    if (section->children[1] != NULL) {
        destroy_ini_section(section->children[1]);
        free((void*)section->children[1]->name);
        free(section->children[1]);
    }
}

IniDefault destroy_ini_file(IniFile file) {
    IniDefault defaults = file.defaults;
    destroy_ini_section(&file.global_section);
    return defaults;
}
