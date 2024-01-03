#ifndef VESTIGE_INI_H
#define VESTIGE_INI_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum IniValueType {
    VALUE_TYPE_NUMBER =  1 << 0,
    VALUE_TYPE_STRING =  1 << 1,
    VALUE_TYPE_BOOLEAN = 1 << 2,
    VALUE_TYPE_UNDECIDED = 0
} IniValueType;

typedef struct IniDefault {
    size_t count;
    struct IniDefaultValue {
        const char* key;
        IniValueType type;
        union {
            double number;
            const char* string;
            bool boolean;
        } value;
    }* pairs;
} IniDefault;

typedef struct IniSection {
    struct IniSection* children[2];
    const char* name;
    struct IniPair {
        const char* key;
        union {
            double number;
            bool boolean;
            char* string;
        };
        IniValueType type;
    }* pairs;
    size_t pair_length;
    size_t pair_size;
} IniSection;

typedef struct IniFile {
    IniDefault defaults;
    IniSection global_section;
} IniFile;

IniFile construct_ini_from_defaults(IniDefault defaults);
IniDefault destroy_ini_file(IniFile file);

double get_number_from_ini(IniFile file, const char* key);
const char* get_string_from_ini(IniFile file, const char* key);
bool get_boolean_from_ini(IniFile file, const char* key);

#endif
