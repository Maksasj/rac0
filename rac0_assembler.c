#include <stdio.h>
#include <stdlib.h>

#define PLUM_IMPLEMENTATION
#include "plum.h"

#include "rac0_parser.h"

int main(int argc, char *argv[]) {
    if(argc < 2) {
        PLUM_LOG(PLUM_ERROR, "File usage: %s <input file name>", argv[0]);
        return 1;
    }

    FILE *stream = fopen(argv[1], "r");

    if(stream == NULL) {
        PLUM_LOG(PLUM_ERROR, "Failed to open file '%s'", argv[1]);
        return 1;
    }

    char *source = NULL;

    /* Go to the end of the file. */
    if (fseek(stream, 0L, SEEK_END) == 0) {
        long buf_size = ftell(stream);

        if (buf_size == -1) { 
            PLUM_LOG(PLUM_ERROR, "Failed ftell for '%s'", argv[1]);
            return 1;
        }

        source = malloc(sizeof(char) * (buf_size + 1));

        if (fseek(stream, 0L, SEEK_SET) != 0)  { 
            PLUM_LOG(PLUM_ERROR, "Failed fseek for '%s'", argv[1]);
            return 1;
        }

        size_t new_len = fread(source, sizeof(char), buf_size, stream);

        if (ferror( stream ) != 0 ) {
            fputs("Error reading file", stderr);
            PLUM_LOG(PLUM_ERROR, "Error reading file '%s'", argv[1]);
            return 1;
        }

        source[new_len++] = '\0'; /* Just to be safe. */
    }

    fclose(stream);

    rac0a_program_t program;

    rac0a_parse_program(&program, source);

    return 0;
}