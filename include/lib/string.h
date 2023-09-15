#ifndef _VESTIGE_LIB_STRING_H
#define _VESTIGE_LIB_STRING_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define _MAX_STRING_LENGTH 16384u

typedef struct String {
	size_t length;
	uint8_t* buffer;  // TODO: small string optimization?
} String;

String new_string(size_t length);
String new_string_from(char* src, size_t length);
String string_from_cstr(char* cstr);
void del_string(String s);

#endif
