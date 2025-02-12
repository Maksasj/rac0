#include <stdio.h>
#include <stdlib.h>

#define PLUM_IMPLEMENTATION
#include "plum.h"

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

    // read file in buffer
    char* buffer = 0;
    long length;

    fseek (stream, 0, SEEK_END);
    length = ftell(stream);
    fseek (stream, 0, SEEK_SET);
    buffer = malloc (length);

    if (buffer == NULL) {
        PLUM_LOG(PLUM_ERROR, "Failed to allocate memory buffer");
        return 1;
    }

    fread(buffer, 1, length, stream);

    return 0;
}