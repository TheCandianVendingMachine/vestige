#define DEFAULT_PAIR_COUNT 8
#define DEFAULT_PATH_LENGTH 4

#include <stdlib.h>
#include <string.h>

#include "ini.h"

struct _IniValue {
    void* value;
    IniValueType type;
};

void insert_into_section(IniSection* section, const char** section_path, const char* key, void* value, int value_type) {
    if (*section_path == NULL) {
        // path is empty, insert key into section
        if (section->pair_length >= section->pair_size) {
            section->pair_size *= 2;
            section->pairs = realloc(section->pairs, sizeof(struct IniPair) * section->pair_size);
        }
        section->pairs[section->pair_length].key = key;
        section->pairs[section->pair_length].type = value_type;
        switch (value_type) {
            case VALUE_TYPE_BOOLEAN:
                section->pairs[section->pair_length].boolean = *(bool*)value;
                break;
            case VALUE_TYPE_STRING: {
                const char** value_str = value;
                size_t value_len = strlen(*value_str);
                section->pairs[section->pair_length].string = malloc(sizeof(char) * (value_len + 1));
                memcpy(section->pairs[section->pair_length].string, *value_str, value_len);
                section->pairs[section->pair_length].string[value_len] = '\0';
                break;
            }
            case VALUE_TYPE_NUMBER:
                section->pairs[section->pair_length].number = *(double*)value;
                break;
            default:
                break;
        }
        section->pair_length += 1;
        return;
    }

    const char* section_name = *section_path;
    bool new_child = false;
    while (!new_child) {
        int direction = strcmp(section_name, section->name);
        if (direction == 0) {
            break;
        }
        if (direction < 0) {
            if (section->children[0] == NULL) {
                section->children[0] = malloc(sizeof(IniSection));
                new_child = true;
            }
            section = section->children[0];
        } else {
            if (section->children[1] == NULL) {
                section->children[1] = malloc(sizeof(IniSection));
                new_child = true;
            }
            section = section->children[1];
        }
    };

    if (new_child) {
        section->pairs = malloc(sizeof(struct IniPair) * DEFAULT_PAIR_COUNT);
        section->pair_length = 0;
        section->pair_size = DEFAULT_PAIR_COUNT;
        section->name = section_name;
        section->children[0] = NULL;
        section->children[1] = NULL;
    } else {
        free((void*)section_name);
    }

    insert_into_section(section, section_path + 1, key, value, value_type);
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
            section_path[path_index] = malloc(sizeof(char) * (built_section_index + 1));
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

void get_path_and_key_from_string(const char* initial, char** key, char** path) {
    size_t key_length = strlen(initial);
    size_t key_split = 0;
    for (int j = key_length - 1; j >= 0; j--) {
        if (initial[j] == '.') {
            key_split = (j >= 0 ? j : 0);
            break;
        }
    }

    *key = malloc(sizeof(char) * (1 + key_length - key_split));
    memset(*key, '\0', sizeof(char) * (1 + key_length - key_split));
    memcpy(*key, initial + key_split + 1, key_length - key_split);

    *path = malloc(sizeof(char) * (key_split + 1));
    memset(*path, '\0', sizeof(char) * (key_split + 1));
    memcpy(*path, initial, key_split);
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
        char* value_key;
        char* value_path;

        get_path_and_key_from_string(defaults.pairs[i].key, &value_key, &value_path);

        const char** section_path = get_section_path_from_string(value_path);

        void* value = NULL;
        switch (defaults.pairs[i].type) {
            case VALUE_TYPE_NUMBER:
                value = &defaults.pairs[i].value.number;
                break;
            case VALUE_TYPE_STRING:
                value = &defaults.pairs[i].value.string;
                break;
            case VALUE_TYPE_BOOLEAN:
                value = &defaults.pairs[i].value.boolean;
                break;
            default:
                break;
        }
        insert_into_section(
            &file.global_section,
            section_path,
            value_key, value,
            defaults.pairs[i].type
        );

        free(section_path);
        free(value_path);
    }

    return file;
}

void destroy_ini_section(IniSection* section) {
    for (int i = 0; i < section->pair_length; i++) {
        free((void*)section->pairs[i].key);
        if (section->pairs[i].type == VALUE_TYPE_STRING) {
            free((void*)section->pairs[i].string);
        }
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

struct _IniValue get_value_from_ini(IniSection* section, const char** path, const char* key) {
    if (*path == NULL) {
        for (int i = 0; i < section->pair_length; i++) {
            if (strcmp(section->pairs[i].key, key) == 0) {
                switch (section->pairs[i].type) {
                    case VALUE_TYPE_BOOLEAN:
                        return (struct _IniValue) {
                            .value = &section->pairs[i].boolean,
                            .type = section->pairs[i].type
                        };
                    case VALUE_TYPE_STRING:
                        return (struct _IniValue) {
                            .value = &section->pairs[i].string,
                            .type = section->pairs[i].type
                        };
                    case VALUE_TYPE_NUMBER:
                        return (struct _IniValue) {
                            .value = &section->pairs[i].number,
                            .type = section->pairs[i].type
                        };
                    default:
                        break;
                };
            }
        }
        return (struct _IniValue) { .value = NULL, .type = VALUE_TYPE_UNDECIDED };
    }

    const char* section_name = *path;
    while (1) {
        int direction = strcmp(section_name, section->name);
        if (direction == 0) {
            break;
        } else if (direction < 0) {
            section = section->children[0];
        } else {
            section = section->children[1];
        }

        if (section == NULL) {
            return (struct _IniValue) { .value = NULL, .type = VALUE_TYPE_UNDECIDED };
        }
    }

    return get_value_from_ini(section, path + 1, key);
}

double get_number_from_ini(IniFile file, const char* key) {
    char* value_key;
    char* value_path;
    get_path_and_key_from_string(key, &value_key, &value_path);
    const char** path = get_section_path_from_string(value_path);

    struct _IniValue value = get_value_from_ini(&file.global_section, path, value_key);

    char** path_iter = (char**)path;
    while (*path_iter) {
        free(*path_iter);
        path_iter += 1;
    }
    free(path);
    free(value_key);
    free(value_path);

    if (!value.value || (value.type & VALUE_TYPE_NUMBER) == 0) { return 0.0; }
    return *(double*)value.value;
}

const char* get_string_from_ini(IniFile file, const char* key) {
    char* value_key;
    char* value_path;
    get_path_and_key_from_string(key, &value_key, &value_path);
    const char** path = get_section_path_from_string(value_path);

    struct _IniValue value = get_value_from_ini(&file.global_section, path, value_key);

    char** path_iter = (char**)path;
    while (*path_iter) {
        free(*path_iter);
        path_iter += 1;
    }
    free(path);
    free(value_key);
    free(value_path);

    if (!value.value || (value.type & VALUE_TYPE_STRING) == 0) { return ""; }
    return *(const char**)value.value;
}

bool get_boolean_from_ini(IniFile file, const char* key) {
    char* value_key;
    char* value_path;
    get_path_and_key_from_string(key, &value_key, &value_path);
    const char** path = get_section_path_from_string(value_path);

    struct _IniValue value = get_value_from_ini(&file.global_section, path, value_key);

    char** path_iter = (char**)path;
    while (*path_iter) {
        free(*path_iter);
        path_iter += 1;
    }
    free(path);
    free(value_key);
    free(value_path);

    if (!value.value || (value.type & VALUE_TYPE_BOOLEAN) == 0) { return false; }
    return *(bool*)value.value;
}
