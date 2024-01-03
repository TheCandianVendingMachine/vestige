#ifndef VESTIGE_DEFAULT_INPUTS
#define VESTIGE_DEFAULT_INPUTS (IniDefault) { \
    .count = 0, \
    .pairs = (struct IniDefaultValue[]) { \
        (struct IniDefaultValue) { .key = "test.double", .type = VALUE_TYPE_NUMBER, .value = { .number = 3.14159 } }, \
    } \
}

#endif
