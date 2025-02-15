#include "rac0a_parser.h"

rac0a_parse_result_t rac0a_parse_token(rac0a_parser_t* parser, rac0a_token_type_t type, rac0a_token_t* ret) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t token = rac0a_next_token(&parser->lexer);

    if(token.type == type) {
        if(ret != NULL)
            *ret = token;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }
    
    parser->lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_exact_word(rac0a_parser_t* parser, const char* lexem) {
    rac0a_lexer_t backup = parser->lexer;
    
    rac0a_token_t token = rac0a_next_token(&parser->lexer);

    if(token.type != RAC0A_TOKEN_LABEL) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(strcmp(token.lexeme, lexem) != 0) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_include_statement(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_AT, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "include").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_token(parser, RAC0A_TOKEN_STRING, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    PLUM_LOG(PLUM_INFO, "Trying to include file");

    char* source = rac0_utils_read_file_string("rac0.sys.asm");

    vector_t child_hl = rac0a_parse_program(source);
    free_vector_content(&child_hl);
    free_vector(&child_hl);

    // todo merge child_program with our program if success

    PLUM_LOG(PLUM_INFO, "Successfully to included file");

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_constval_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_AT, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "constval").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(parser, RAC0A_TOKEN_NUMBER, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_label_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_token(parser, RAC0A_TOKEN_COLON, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_eof(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_EOF, NULL);
}

rac0a_parse_result_t rac0a_parse_number(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_NUMBER, NULL);
}

rac0a_parse_result_t rac0a_parse_label_usage(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_AMPERSAND, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_instruction_noarg(rac0a_parser_t* parser, const char* lexem) {
    return rac0a_parse_exact_word(parser, lexem);
}

rac0a_parse_result_t rac0a_parse_instruction_arg(rac0a_parser_t* parser, const char* lexem, rac0_value_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_exact_word(parser, lexem).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_number(parser).code == RAC0A_OK) {
        *value = 0xaaaaaaaaaaaaaaaF; 
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_label_usage(parser).code == RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_OK };

    parser->lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_instruction(rac0a_parser_t* parser, rac0_inst_t* inst) {
    rac0a_lexer_t backup = parser->lexer;

    rac0_value_t value = 0x0;

    if(rac0a_parse_instruction_noarg(parser, "halt").code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "HALT instruction definition");
        
        inst->opcode = RAC0_HALT_OPCODE;
        inst->value = value;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction_arg(parser, "pusha", &value).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "PUSHA instruction definition");

        inst->opcode = RAC0_PUSHA_OPCODE;
        inst->value = value;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction_arg(parser, "addat", &value).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "ADDAT instruction definition");

        inst->opcode = RAC0_ADDAT_OPCODE;
        inst->value = value;
        
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction_arg(parser, "jmpga", &value).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "JMPGA instruction definition");

        inst->opcode = RAC0_JMPGA_OPCODE;
        inst->value = value;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    parser->lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_byte_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "db").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(parser, RAC0A_TOKEN_STRING, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_word_definition(rac0a_parser_t* parser, rac0_value_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "dw").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(parser, RAC0A_TOKEN_NUMBER, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    *value = 0xaaaaaaaaaaaaaaaF;

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_statement_list(rac0a_parser_t* parser) {
    while(1) {
        rac0_inst_t inst;
        rac0_value_t value;

        if(rac0a_parse_include_statement(parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Include definition");        
        } else if(rac0a_parse_constval_definition(parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Constval definition");

            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_CONSTVAL;
            vector_push(&parser->hl_statements, st);
        } else if(rac0a_parse_label_definition(parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Label definition");

            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_LABEL;
            vector_push(&parser->hl_statements, st);
        } else if(rac0a_parse_byte_definition(parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Byte definition");

            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_BYTE_DEF;
            vector_push(&parser->hl_statements, st);
        } else if(rac0a_parse_word_definition(parser, &value).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Word definition");

            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_WORD_DEF;
            st->as.word_def.value = value;

            vector_push(&parser->hl_statements, st);
        } else if(rac0a_parse_module(parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Module usage");
        } else if(rac0a_parse_instruction(parser, &inst).code == RAC0A_OK) {
            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_INSTRUCTION;
            st->as.instruction.inst = inst;

            vector_push(&parser->hl_statements, st);
        } else if(rac0a_parse_token(parser, RAC0A_TOKEN_EOF, NULL).code == RAC0A_OK) {
            --parser->lexer.pointer;
            break;
        } else if(rac0a_parse_token(parser, RAC0A_TOKEN_R_BRACKET, NULL).code == RAC0A_OK) {
            --parser->lexer.pointer;
            break;
        } else {
            return (rac0a_parse_result_t) { RAC0A_ERROR };
        }
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_module(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_AT, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "module").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(parser, RAC0A_TOKEN_L_BRACKET, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_statement_list(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_token(parser, RAC0A_TOKEN_R_BRACKET, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

vector_t rac0a_parse_program(const char* input) {
    rac0a_parser_t parser = {
        .lexer = (rac0a_lexer_t) {
            .input = input,
            .pointer = 0
        }
    };

    create_vector(&parser.hl_statements, 1024);

    while(1) {
        if(rac0a_parse_eof(&parser).code == RAC0A_OK) {
            break;
        } else if(rac0a_parse_statement_list(&parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Statement list");
            continue;
        } else {
            PLUM_LOG(PLUM_ERROR, "Dead end");
            break;
        }
    }

    return parser.hl_statements;
}
