#ifndef RAC0_UTILS_H
#define RAC0_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rac0_base.h"

typedef char* string_t;
typedef unsigned char rac0a_boolean_t;

char* rac0_utils_read_file_bytes(char* file_name);
char* rac0_utils_read_file_string(char* file_name);

char* rac0a_string_copy(const char* input);
char* rac0a_string_copy_len(const char* input, unsigned long long size);

rac0a_boolean_t rac0a_in_range_include(char value, char min, char max);

#endif
