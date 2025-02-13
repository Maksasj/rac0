#ifndef RAC0_PARSER_H
#define RAC0_PARSER_H

#define HAUL_IMPLEMENTATION
#include "haul/haul.h"

#include "rac0_lexer.h"

typedef struct {
    vector_t macro_def;
    vector_t instructions;
} rac0a_program_t;

typedef struct {
    rac0a_result_code_t code;
} rac0a_parse_result_t;

rac0a_parse_result_t rac0a_parse_token(rac0a_lexer_t* lexer, rac0a_token_type_t type) {
    rac0a_lexer_t backup = *lexer;

    rac0a_token_t token = rac0a_next_token(lexer);

    if(token.type == type) {
        return (rac0a_parse_result_t) { RAC0A_OK };
    }
    
    *lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0_parse_macro_definition(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;
    
    rac0a_token_t token = rac0a_next_token(lexer);

    if(token.type != RAC0A_TOKEN_AT) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);
    token = rac0a_next_token(lexer);
    
    if(token.type != RAC0A_TOKEN_LABEL) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0_parse_label(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;
    
    rac0a_token_t token = rac0a_next_token(lexer);

    if(token.type != RAC0A_TOKEN_DOLLAR) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);
    token = rac0a_next_token(lexer);
    
    if(token.type != RAC0A_TOKEN_LABEL) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);
    token = rac0a_next_token(lexer);
    
    if(token.type != RAC0A_TOKEN_COLON) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0_parse_eof(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;
    
    rac0a_token_t token = rac0a_next_token(lexer);

    if(token.type != RAC0A_TOKEN_EOF) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0_parse_instruction_noarg(rac0a_lexer_t* lexer, const char* lexem) {
    rac0a_lexer_t backup = *lexer;
    
    rac0a_token_t token = rac0a_next_token(lexer);

    if(token.type != RAC0A_TOKEN_LABEL) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(strcmp(token.lexeme, lexem) != 0) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0_parse_number(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;
    
    rac0a_token_t token = rac0a_next_token(lexer);

    if(token.type != RAC0A_TOKEN_NUMBER) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0_parse_label_ref(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;
    
    rac0a_token_t token = rac0a_next_token(lexer);

    if(token.type != RAC0A_TOKEN_DOLLAR) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);
    token = rac0a_next_token(lexer);
    
    if(token.type != RAC0A_TOKEN_LABEL) {
        *lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0_parse_instruction_arg(rac0a_lexer_t* lexer, const char* lexem) {
    if(rac0_parse_label(lexer).code != RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_ERROR };

    if(rac0_parse_number(lexer).code == RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_OK };

    if(rac0_parse_label_ref(lexer).code == RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_OK };

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0_parse_instruction(rac0a_lexer_t* lexer) {
    if(rac0_parse_label(lexer).code == RAC0A_OK) {
        PLUM_LOG(PLUM_INFO, "Label definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    // cpu instruction
    if(rac0_parse_instruction_noarg(lexer, "halt").code == RAC0A_OK) {
        PLUM_LOG(PLUM_INFO, "HALT instruction definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0_parse_instruction_arg(lexer, "pusha").code == RAC0A_OK) {
        PLUM_LOG(PLUM_INFO, "PUSHA instruction definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

void rac0a_parse_program(rac0a_program_t* program, const char* input) {
    rac0a_lexer_t lexer = (rac0a_lexer_t) {
        .input = input,
        .pointer = 0
    };

    while(1) {
        if(rac0_parse_macro_definition(&lexer).code == RAC0A_OK) {
            PLUM_LOG(PLUM_INFO, "Macro definition");
            continue;
        } else if(rac0_parse_instruction(&lexer).code == RAC0A_OK) {
            continue;
        } else if(rac0_parse_eof(&lexer).code == RAC0A_OK) {
            break;
        } else {
            PLUM_LOG(PLUM_ERROR, "Dead end");
            break;
        }
    }
}

#endif
