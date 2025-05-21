#include "rac0_utils.h"

char* rac0_utils_read_file_bytes(char* file_name) {
    FILE *stream = fopen(file_name, "rb");

    if(stream == NULL) {
        PLUM_LOG(PLUM_ERROR, "Failed to open file '%s'", file_name);
        return NULL;
    }

    char *source = NULL;

    /* Go to the end of the file. */
    if (fseek(stream, 0L, SEEK_END) == 0) {
        long buf_size = ftell(stream);

        if (buf_size == -1) { 
            PLUM_LOG(PLUM_ERROR, "Failed ftell for '%s'", file_name);
            return NULL;
        }

        source = (char*) malloc(sizeof(char) * buf_size);

        if (fseek(stream, 0L, SEEK_SET) != 0)  { 
            PLUM_LOG(PLUM_ERROR, "Failed fseek for '%s'", file_name);
            return NULL;
        }

        size_t new_len = fread(source, sizeof(char), buf_size, stream);

        if (ferror( stream ) != 0 ) {
            fputs("Error reading file", stderr);
            PLUM_LOG(PLUM_ERROR, "Error reading file '%s'", file_name);
            return NULL;
        }
    }

    fclose(stream);

    return source;
}


char* rac0_utils_read_file_string(char* file_name) {
    FILE *stream = fopen(file_name, "r");

    if(stream == NULL) {
        // PLUM_LOG(PLUM_ERROR, "Failed to open file '%s'", file_name);
        return NULL;
    }

    char *source = NULL;

    /* Go to the end of the file. */
    if (fseek(stream, 0L, SEEK_END) == 0) {
        long buf_size = ftell(stream);

        if (buf_size == -1) { 
            // PLUM_LOG(PLUM_ERROR, "Failed ftell for '%s'", file_name);
            return NULL;
        }

        source = (char*) malloc(sizeof(char) * (buf_size + 1));

        if (fseek(stream, 0L, SEEK_SET) != 0)  { 
            // PLUM_LOG(PLUM_ERROR, "Failed fseek for '%s'", file_name);
            return NULL;
        }

        size_t new_len = fread(source, sizeof(char), buf_size, stream);

        if (ferror( stream ) != 0 ) {
            fputs("Error reading file", stderr);
            // PLUM_LOG(PLUM_ERROR, "Error reading file '%s'", file_name);
            return NULL;
        }

        source[new_len++] = '\0'; /* Just to be safe. */
    }

    fclose(stream);

    return source;
}

char* rac0a_string_copy(const char* input) {
    unsigned long long len = strlen(input) + 1;
    char* out =  malloc(sizeof(char) * len);
    memcpy(out, input, len);
    return out;
}

char* rac0a_string_copy_len(const char* input, unsigned long long size) {
    unsigned long long len = size + 1;
    char* out =  calloc(len, sizeof(char));
    memcpy(out, input, size);
    return out;
}

rac0_u64_t rac0a_get_line_number_from_index(const char* input, unsigned long long index) {
    rac0_u64_t line = 0;

    for(int i = 0; i < index; ++i) {
        if(input[i] == '\n')
            ++line;
    }

    return line;
}

rac0_u64_t rac0a_get_line_length(const char* input) {
    unsigned long long length = strlen(input);
    rac0_u64_t pointer = 0;

    for(int i = 0; i < length; ++i) {
        if(input[i] == '\n')
            break;

        ++pointer;
    }


    return pointer;
}

rac0a_boolean_t rac0a_in_range_include(char value, char min, char max) {
    return (value >= min) && (value <= max); 
}
