#include <stdio.h>
#include <stdlib.h>

#define PLUM_IMPLEMENTATION
#include "plum.h"

#include "rac0_assembler.h"

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
    buffer = calloc(length, 0);

    if (buffer == NULL) {
        PLUM_LOG(PLUM_ERROR, "Failed to allocate memory buffer");
        return 1;
    }

    fread(buffer, 1, length, stream);

    buffer[length] = '\0';

    PLUM_LOG(PLUM_TRACE, "Bufffer '%s'", buffer);

    rac0a_lexer_t lexer = (rac0a_lexer_t) {
        .input = buffer,
        .pointer = 0
    };

    while(1) {
        rac0a_token_t token = rac0a_next_token(&lexer);  

        if(token.type == RAC0A_TOKEN_EOF)
            break;

        if(token.type == RAC0A_TOKEN_ERROR) {
            PLUM_LOG(PLUM_ERROR, "Unknown token at '%d'", lexer.pointer);
            break;
        }

        PLUM_LOG(PLUM_TRACE, "Token '%d': '%s'", token.type, token.lexeme);

        rac0a_free_token(token);
    }

    return 0;
}