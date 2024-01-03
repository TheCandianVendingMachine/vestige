#ifndef VESTIGE_DEFAULT_INPUTS
#define VESTIGE_DEFAULT_INPUTS (IniDefault) { \
    .count = 7, \
    .pairs = (struct IniDefaultValue[]) { \
        (struct IniDefaultValue) { .key = "a.b.c.d.nested", .type = VALUE_TYPE_STRING, .value = { .string = "nested" } }, \
        (struct IniDefaultValue) { .key = "global", .type = VALUE_TYPE_STRING, .value = { .string = "global" } }, \
        (struct IniDefaultValue) { .key = "a.b.sub", .type = VALUE_TYPE_STRING, .value = { .string = "sub" } }, \
        (struct IniDefaultValue) { .key = "a.b.d.alt", .type = VALUE_TYPE_STRING, .value = { .string = "alt" } }, \
        (struct IniDefaultValue) { .key = "a.b.c.d.nested", .type = VALUE_TYPE_STRING, .value = { .string = "duplicate" } }, \
        (struct IniDefaultValue) { .key = "test.sub.bool", .type = VALUE_TYPE_BOOLEAN, .value = { .boolean = true } }, \
        (struct IniDefaultValue) { .key = "test.double", .type = VALUE_TYPE_NUMBER, .value = { .number = 3.14159 } }, \
    } \
}

#endif
