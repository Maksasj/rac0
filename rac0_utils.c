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

        source = (char*) malloc(sizeof(char) * (buf_size + 1));

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

        source[new_len++] = '\0'; /* Just to be safe. */
    }

    fclose(stream);

    return source;
}
