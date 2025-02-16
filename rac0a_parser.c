#include "rac0a_parser.h"

// parsing utils
rac0a_parse_result_t rac0a_parse_token(rac0a_parser_t* parser, rac0a_token_type_t type, rac0a_token_t* ret) {
    rac0a_lexer_t backup = parser->lexer;

    // there we skip comments
    while (1) {
        rac0a_token_t token = rac0a_next_token(&parser->lexer);
        
        if(token.type == RAC0A_TOKEN_COMMENT)
            continue;

        if(token.type == type) {
            if(ret != NULL)
                *ret = token;
            else
                rac0a_free_token(token);

            return (rac0a_parse_result_t) { RAC0A_OK };
        }

        break;
    }

    parser->lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_exact_word(rac0a_parser_t* parser, const char* lexem) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code != RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_ERROR };

    if(strcmp(token.lexeme, lexem) != 0) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

// parsing shortcuts
rac0a_parse_result_t rac0a_parse_at(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_AT, NULL);
}

rac0a_parse_result_t rac0a_parse_dollar(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_DOLLAR, NULL);
}

rac0a_parse_result_t rac0a_parse_l_paren(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_L_PAREN, NULL);
}

rac0a_parse_result_t rac0a_parse_r_paren(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_R_PAREN, NULL);
}

rac0a_parse_result_t rac0a_parse_colon(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_COLON, NULL);
}

rac0a_parse_result_t rac0a_parse_label(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, NULL);
}

rac0a_parse_result_t rac0a_parse_number(rac0a_parser_t* parser, rac0_value_t* number) {
    rac0a_token_t token;

    // todo add support for float binary and decimal numbers
    if(rac0a_parse_token(parser, RAC0A_TOKEN_NUMBER, &token).code == RAC0A_OK) {
        *number = strtoull(token.lexeme, NULL, 0);
        
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    rac0a_free_token(token);
    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_l_bracket(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_L_BRACKET, NULL);
}

rac0a_parse_result_t rac0a_parse_r_bracket(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_R_BRACKET, NULL);
}

rac0a_parse_result_t rac0a_parse_percent(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_PERCENT, NULL);
}

rac0a_parse_result_t rac0a_parse_string(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_STRING, NULL);
}

rac0a_parse_result_t rac0a_parse_ampersand(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_AMPERSAND, NULL);
}

rac0a_parse_result_t rac0a_parse_comment(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_COMMENT, NULL);
}

rac0a_parse_result_t rac0a_parse_eof(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_EOF, NULL);
}

// other
rac0a_parse_result_t rac0a_parse_include_statement(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_at(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "include").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    // todo
    if(rac0a_parse_token(parser, RAC0A_TOKEN_STRING, NULL).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    PLUM_LOG(PLUM_INFO, "Trying to include file");

    char* source = rac0_utils_read_file_string("rac0.sys.asm");

    vector_t child_hl = rac0a_parse_program(source);

    for(int i = 0; i < vector_size(&child_hl); ++i) {
        vector_push(&parser->hl_statements, vector_get(&child_hl, i)); 
    }

    free_vector(&child_hl);

    PLUM_LOG(PLUM_INFO, "Successfully to included file");

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_constval_definition(rac0a_parser_t* parser, rac0a_hl_constval_statement_t* constval) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_at(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "constval").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    rac0a_token_t token;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0_value_t value;
    if(rac0a_parse_number(parser, &value).code != RAC0A_OK) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    constval->value = value;
    constval->label = rac0a_string_copy(token.lexeme);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_constblock_definition(rac0a_parser_t* parser, rac0a_hl_constblock_statement_t* block) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_at(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "constblock").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_token_t token;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_l_paren(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_r_paren(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_l_bracket(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    vector_t constblock_statements;
    create_vector(&constblock_statements, 100);

    if(rac0a_parse_statement_list(parser, &constblock_statements).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_r_bracket(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    block->label = rac0a_string_copy(token.lexeme);
    block->statements = constblock_statements;

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_label_definition(rac0a_parser_t* parser, rac0a_hl_label_statement_t* label) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t token;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_colon(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    label->label = token.lexeme;

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_const_thing_usage(rac0a_parser_t* parser, char** label) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_dollar(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_token_t token;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    *label = token.lexeme;

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_label_pointer(rac0a_parser_t* parser, char** label) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_ampersand(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    *label = token.lexeme;

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_value(rac0a_parser_t* parser, rac0a_hl_value_t* value) {
    rac0_value_t number;
    char* label;

    if(rac0a_parse_number(parser, &number).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_NUMBER;
        value->as.value = number;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_const_thing_usage(parser, &label).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_CONSTVAL;
        value->as.constval_label = label;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_label_pointer(parser, &label).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_LABEL_POINTER;
        value->as.label = label;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_instruction_noarg(rac0a_parser_t* parser, const char* lexem) {
    return rac0a_parse_exact_word(parser, lexem);
}

rac0a_parse_result_t rac0a_parse_instruction_arg(rac0a_parser_t* parser, const char* lexem, rac0a_hl_value_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_exact_word(parser, lexem).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_value(parser, value).code == RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_OK };

    parser->lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_instruction(rac0a_parser_t* parser, rac0a_hl_instruction_statement_t* inst) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_hl_value_t value;

    if(rac0a_parse_instruction_noarg(parser, "halt").code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "HALT instruction definition");

        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;
        inst->inst.as.opcode = RAC0_HALT_OPCODE;
        inst->inst.as.opcode = RAC0_HALT_OPCODE;
        inst->value.type = RAC0A_HL_VALUE_TYPE_NONE;
        
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction_arg(parser, "pushda", &value).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "PUSHDA instruction definition");

        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;
        inst->inst.as.opcode = RAC0_PUSHDA_OPCODE;
        inst->value = value;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction_arg(parser, "pusha", &value).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "PUSHA instruction definition");

        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;
        inst->inst.as.opcode = RAC0_PUSHA_OPCODE;
        inst->value = value;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction_arg(parser, "addat", &value).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "ADDAT instruction definition");

        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;
        inst->inst.as.opcode = RAC0_ADDAT_OPCODE;
        inst->value = value;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_instruction_arg(parser, "jmpga", &value).code == RAC0A_OK) {
        PLUM_LOG(PLUM_TRACE, "JMPGA instruction definition");

        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;
        inst->inst.as.opcode = RAC0_JMPGA_OPCODE;
        inst->value = value;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    parser->lexer = backup;

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_byte_definition(rac0a_parser_t* parser, rac0a_hl_byte_def_statement_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t label;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &label).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "db").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_token_t token;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_STRING, &token).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    value->label = label.lexeme;
    value->array = token.lexeme;
    value->size = strlen(token.lexeme);
    
    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_word_definition(rac0a_parser_t* parser, rac0a_hl_word_def_statement_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t label;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &label).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "dw").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0_value_t number;

    if(rac0a_parse_number(parser, &number).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    value->value = number;
    value->label = label.lexeme;

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_statement_list(rac0a_parser_t* parser, vector_t* list) {
    while(1) {
        rac0a_hl_constblock_statement_t constblock;
        rac0a_hl_constval_statement_t constval;
        rac0a_hl_instruction_statement_t inst;
        rac0a_hl_label_statement_t label;
        rac0a_hl_word_def_statement_t word_def;
        rac0a_hl_byte_def_statement_t byte_def;

        if(rac0a_parse_include_statement(parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Include definition");        
        } else if(rac0a_parse_constval_definition(parser, &constval).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Constval definition");

            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_CONSTVAL_DECL;
            st->as.constval = constval;
            
            vector_push(list, st);
        } else if(rac0a_parse_constblock_definition(parser, &constblock).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Constblock definition");

            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_CONSTBLOCK_DECL;
            st->as.constblock = constblock;
            
            vector_push(&parser->hl_statements, st);
        } else if(rac0a_parse_label_definition(parser, &label).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Label definition");
            
            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_LABEL;
            st->as.label = label;

            vector_push(list, st);
        } else if(rac0a_parse_byte_definition(parser, &byte_def).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Byte definition");

            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_BYTE_DEF;
            st->as.byte_def = byte_def;
            vector_push(list, st);
        } else if(rac0a_parse_word_definition(parser, &word_def).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Word definition");

            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_WORD_DEF;
            st->as.word_def = word_def;

            vector_push(list, st);
        } else if(rac0a_parse_module_definition(parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Module usage");
        } else if(rac0a_parse_constblock_usage(parser).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Constblock usage");
        } else if(rac0a_parse_instruction(parser, &inst).code == RAC0A_OK) {
            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->type = RAC0A_HL_TYPE_INSTRUCTION;
            st->as.instruction = inst;

            vector_push(list, st);
        } else if(rac0a_parse_token(parser, RAC0A_TOKEN_EOF, NULL).code == RAC0A_OK) {
            --parser->lexer.pointer;
            break;
        } else if(rac0a_parse_r_bracket(parser).code == RAC0A_OK) {
            --parser->lexer.pointer;
            break;
        } else {
            return (rac0a_parse_result_t) { RAC0A_ERROR };
        }
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_constblock_usage(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_dollar(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_label(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_l_paren(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_r_paren(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_module_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_at(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_exact_word(parser, "module").code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_label(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_l_bracket(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_statement_list(parser, &parser->hl_statements).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_r_bracket(parser).code != RAC0A_OK) {
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
        } else if(rac0a_parse_statement_list(&parser, &parser.hl_statements).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Statement list");
            continue;
        } else {
            PLUM_LOG(PLUM_ERROR, "Dead end");
            break;
        }
    }

    return parser.hl_statements;
}
