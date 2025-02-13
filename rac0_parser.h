#ifndef RAC0_PARSER_H
#define RAC0_PARSER_H

#define HAUL_IMPLEMENTATION
#include "haul/haul.h"

#include "rac0_lexer.h"

typedef struct {
    vector_t modules;
} rac0a_program_t;

typedef struct {
    rac0a_result_code_t code;
} rac0a_parse_result_t;

rac0a_parse_result_t rac0a_parse_token(rac0a_lexer_t* lexer, rac0a_token_type_t type) {
    rac0a_lexer_t backup = *lexer;

    rac0a_token_t token = rac0a_next_token(lexer);

    if(token.type == type)
        return (rac0a_parse_result_t) { RAC0A_OK };
    
    *lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_exact_word(rac0a_lexer_t* lexer, const char* lexem) {
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

rac0a_parse_result_t rac0a_parse_include_statement(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_AT).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(lexer, "include").code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_token(lexer, RAC0A_TOKEN_STRING).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_constval_definition(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_AT).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(lexer, "consteval").code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_token(lexer, RAC0A_TOKEN_LABEL).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_NUMBER).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_label_definition(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_DOLLAR).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_LABEL).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_COLON).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_eof(rac0a_lexer_t* lexer) {
    return rac0a_parse_token(lexer, RAC0A_TOKEN_EOF);
}

rac0a_parse_result_t rac0a_parse_number(rac0a_lexer_t* lexer) {
    return rac0a_parse_token(lexer, RAC0A_TOKEN_NUMBER);
}

rac0a_parse_result_t rac0a_parse_label_usage(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_DOLLAR).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_LABEL).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_instruction_noarg(rac0a_lexer_t* lexer, const char* lexem) {
    return rac0a_parse_exact_word(lexer, lexem);
}

rac0a_parse_result_t rac0a_parse_instruction_arg(rac0a_lexer_t* lexer, const char* lexem) {
    rac0a_lexer_t backup = *lexer;

    if(rac0a_parse_exact_word(lexer, lexem).code != RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_ERROR };

    if(rac0a_parse_number(lexer).code == RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_OK };

    if(rac0a_parse_label_usage(lexer).code == RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_OK };

    *lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_instruction(rac0a_lexer_t* lexer) {
    if(rac0a_parse_instruction_noarg(lexer, "halt").code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "HALT instruction definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction_arg(lexer, "pusha").code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "PUSHA instruction definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_byte_definition(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_LABEL).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(lexer, "db").code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_STRING).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_word_definition(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_LABEL).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(lexer, "dw").code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_NUMBER).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_statement(rac0a_lexer_t* lexer) {
    if(rac0a_parse_include_statement(lexer).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "Include definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_constval_definition(lexer).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "Constval definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_label_definition(lexer).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "Label definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_byte_definition(lexer).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "Byte definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_word_definition(lexer).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "Word definition");
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction(lexer).code == RAC0A_OK) {
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_statement_list(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;

    while(1) {
        if(rac0a_parse_statement(lexer).code == RAC0A_OK)
            continue;
        else {
            break;
        }
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_module(rac0a_lexer_t* lexer) {
    rac0a_lexer_t backup = *lexer;

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_AT).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(lexer, "module").code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_LABEL).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_L_BRACKET).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_statement_list(lexer).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(lexer, RAC0A_TOKEN_R_BRACKET).code != RAC0A_OK) {
        *lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

void rac0a_parse_program(rac0a_program_t* program, const char* input) {
    rac0a_lexer_t lexer = (rac0a_lexer_t) {
        .input = input,
        .pointer = 0
    };

    create_vector(&program->modules, 10);

    while(1) {
        if(rac0a_parse_module(&lexer).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Module definition");
            continue;
        } else if(rac0a_parse_eof(&lexer).code == RAC0A_OK) {
            break;
        } else {
            PLUM_LOG(PLUM_ERROR, "Dead end");
            break;
        }
    }
}

#endif
