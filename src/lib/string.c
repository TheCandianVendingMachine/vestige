#include <lib/string.h>

String new_string(size_t length) {
    uint8_t* buffer = malloc(length);
    if (buffer == NULL) {
        perror("new_string: ");
        exit(1);
    }
    memset(buffer, '\0', length);
    String s = {
        .length = length,
        .buffer = buffer,
    };
    return s;
}

String new_string_from(char* src, size_t length) {
    uint8_t* buffer = malloc(length);
    if (buffer == NULL) {
        perror("new_string_from: ");
        exit(1);
    }
    strncpy((char*)buffer, src, length);
    String s = {
        .length = length,
        .buffer = buffer,
    };
    return s;
}

static size_t safe_strlen(char* cstr, size_t max_length) {
    char* start = cstr;
    while (*cstr != '\0' && cstr - start < max_length)
        cstr++;
    return cstr - start;
}

String string_from_cstr(char* cstr) {
    size_t length = safe_strlen(cstr, _MAX_STRING_LENGTH);
    return new_string_from(cstr, length);
}

void del_string(String s) {
    free(s.buffer);
}
