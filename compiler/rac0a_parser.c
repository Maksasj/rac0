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

rac0a_parse_result_t rac0a_parse_exact_word(rac0a_parser_t* parser, const string_t lexem) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code != RAC0A_OK) {
        parser->lexer = backup;
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
    // todo add support for float binary and decimal numbers

    rac0a_token_t token;
    token.lexeme = NULL;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_NUMBER, &token).code == RAC0A_OK) {
        *number = strtoull(token.lexeme, NULL, 0);
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

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

rac0a_parse_result_t rac0a_parse_string(rac0a_parser_t* parser, string_t* string) {
    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_STRING, &token).code == RAC0A_OK) {
        *string = rac0a_string_copy(token.lexeme);
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_ampersand(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_AMPERSAND, NULL);
}

rac0a_parse_result_t rac0a_parse_comment(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_COMMENT, NULL);
}

rac0a_parse_result_t rac0a_parse_comma(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_COMMA, NULL);
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

    string_t include_path;
    if(rac0a_parse_string(parser, &include_path).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    PLUM_LOG(PLUM_INFO, "Trying to include file '%s'", include_path);

    string_t source = rac0_utils_read_file_string(include_path);
        free(include_path);

    if(source == NULL) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    rac0a_hl_statement_list_t child_hl = rac0a_parse_program(source);

    for(int i = 0; i < vector_size(&child_hl); ++i)
        vector_push(&parser->hl_statements, vector_get(&child_hl, i)); 

    free_vector(&child_hl); // we dont want to free child hl statements

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

    constval->label = rac0a_string_copy(token.lexeme);
    constval->value = value;

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_constval_argument_definition(rac0a_parser_t* parser) {
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

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };   
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_constblock_argument_definition(rac0a_parser_t* parser) {
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

    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };   
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_argument_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_constblock_constval_argument_definition(parser).code == RAC0A_OK) {
        return (rac0a_parse_result_t) { RAC0A_OK };  
    }
        
    if(rac0a_parse_constblock_constblock_argument_definition(parser).code == RAC0A_OK) {
        return (rac0a_parse_result_t) { RAC0A_OK };  
    }

    parser->lexer = backup;
    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_constblock_argument_list_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    while (1) {
        if(rac0a_parse_constblock_argument_definition(parser).code == RAC0A_OK) {
            if(rac0a_parse_comma(parser).code == RAC0A_OK)
                continue;
        }
        
        if((rac0a_parse_r_paren(parser).code == RAC0A_OK))
            break;

        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

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
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_constblock_argument_list_definition(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    // already handled by rac0a_parse_constblock_argument_list_definition
    // if(rac0a_parse_r_paren(parser).code != RAC0A_OK) {
    //     parser->lexer = backup;
    //     rac0a_free_token(token);
    //     return (rac0a_parse_result_t) { RAC0A_ERROR };
    // }
    
    if(rac0a_parse_l_bracket(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    rac0a_hl_statement_list_t constblock_statements;
    create_vector(&constblock_statements, 100);

    if(rac0a_parse_statement_list(parser, &constblock_statements).code != RAC0A_OK) {
        parser->lexer = backup;
        rac0a_free_token(token);
        free_vector(&constblock_statements);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_r_bracket(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        rac0a_free_token(token);
        rac0a_free_hl_statement_list(&constblock_statements);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    block->label = rac0a_string_copy(token.lexeme);
    block->statements = constblock_statements;

    rac0a_free_token(token);

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
        rac0a_free_token(token);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    label->label = rac0a_string_copy(token.lexeme);
    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_const_thing_usage(rac0a_parser_t* parser, string_t* label) {
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

    *label = rac0a_string_copy(token.lexeme);
    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_label_pointer(rac0a_parser_t* parser, string_t* label) {
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

    *label = rac0a_string_copy(token.lexeme);
    rac0a_free_token(token);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_value(rac0a_parser_t* parser, rac0a_hl_value_t* value) {
    rac0_value_t number;
    string_t label;

    if(rac0a_parse_number(parser, &number).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_NUMBER;
        value->as.value = number;

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_const_thing_usage(parser, &label).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_CONSTVAL;
        value->as.constval_label = rac0a_string_copy(label);

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    if(rac0a_parse_label_pointer(parser, &label).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_LABEL_POINTER;
        value->as.label = label = rac0a_string_copy(label);

        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

rac0a_parse_result_t rac0a_parse_instruction_noarg(rac0a_parser_t* parser, const string_t lexem) {
    return rac0a_parse_exact_word(parser, lexem);
}

rac0a_parse_result_t rac0a_parse_instruction_arg(rac0a_parser_t* parser, const string_t lexem, rac0a_hl_value_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_exact_word(parser, lexem).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    if(rac0a_parse_value(parser, value).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

#define PARSE_INSTRUCTION_NOARG(label, operation_code)                              \
    if(rac0a_parse_instruction_noarg(parser, label).code == RAC0A_OK) {             \
        PLUM_LOG(PLUM_TRACE, label " instruction definition");                      \
                                                                                    \
        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;                         \
        inst->inst.as.opcode = operation_code;                                      \
        inst->value.type = RAC0A_HL_VALUE_TYPE_NONE;                                \
                                                                                    \
        return (rac0a_parse_result_t) { RAC0A_OK };                                 \
    }                                                                               \

#define PARSE_INSTRUCTION_ARG(label, operation_code)                                \
    if(rac0a_parse_instruction_arg(parser, label, &value).code == RAC0A_OK) {       \
        PLUM_LOG(PLUM_TRACE, label "instruction definition");                       \
                                                                                    \
        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;                         \
        inst->inst.as.opcode = operation_code;                                      \
        inst->value = value;                                                        \
                                                                                    \
        return (rac0a_parse_result_t) { RAC0A_OK };                                 \
    }                                                                               \

rac0a_parse_result_t rac0a_parse_instruction(rac0a_parser_t* parser, rac0a_hl_instruction_statement_t* inst) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_hl_value_t value;

    // cpu
    PARSE_INSTRUCTION_NOARG("halt", RAC0_HALT_OPCODE)
    PARSE_INSTRUCTION_NOARG("wait", RAC0_WAIT_OPCODE)
    PARSE_INSTRUCTION_NOARG("setidtt", RAC0_SETIDTT_OPCODE)
    PARSE_INSTRUCTION_NOARG("setidtst", RAC0_SETIDTST_OPCODE)
    PARSE_INSTRUCTION_NOARG("setptbat", RAC0_SETPTBAT_OPCODE)
    PARSE_INSTRUCTION_NOARG("setptst", RAC0_SETPTST_OPCODE)
    PARSE_INSTRUCTION_NOARG("setptpst", RAC0_SETPTPST_OPCODE)
    PARSE_INSTRUCTION_NOARG("settt", RAC0_SETTT_OPCODE)
    PARSE_INSTRUCTION_NOARG("setstt", RAC0_SETSTT_OPCODE)

    // stack
    PARSE_INSTRUCTION_ARG("pusha", RAC0_PUSHA_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushpc", RAC0_PUSHPC_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushss", RAC0_PUSHSS_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushdc", RAC0_PUSHDC_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushms", RAC0_PUSHMS_OPCODE);
    PARSE_INSTRUCTION_NOARG("dupt", RAC0_DUPT_OPCODE);
    PARSE_INSTRUCTION_NOARG("dupn", RAC0_DUPN_OPCODE);
    PARSE_INSTRUCTION_NOARG("drop", RAC0_DROP_OPCODE);
    PARSE_INSTRUCTION_NOARG("swap", RAC0_SWAP_OPCODE);

    // memory
    PARSE_INSTRUCTION_NOARG("store", RAC0_STORE_OPCODE);
    PARSE_INSTRUCTION_ARG("storea", RAC0_STOREA_OPCODE);
    PARSE_INSTRUCTION_NOARG("load", RAC0_LOAD_OPCODE);
    PARSE_INSTRUCTION_ARG("loada", RAC0_LOADA_OPCODE);

    // arithmetic
    PARSE_INSTRUCTION_NOARG("add", RAC0_ADD_OPCODE);
    PARSE_INSTRUCTION_NOARG("sub", RAC0_SUB_OPCODE);
    PARSE_INSTRUCTION_NOARG("mul", RAC0_MUL_OPCODE);
    PARSE_INSTRUCTION_NOARG("div", RAC0_DIV_OPCODE);
    PARSE_INSTRUCTION_NOARG("mod", RAC0_MOD_OPCODE);

    // logic
    PARSE_INSTRUCTION_NOARG("cmp", RAC0_CMP_OPCODE);
    PARSE_INSTRUCTION_NOARG("neg", RAC0_NEG_OPCODE);
    PARSE_INSTRUCTION_NOARG("not", RAC0_NOT_OPCODE);
    PARSE_INSTRUCTION_NOARG("and", RAC0_AND_OPCODE);
    PARSE_INSTRUCTION_NOARG("or", RAC0_OR_OPCODE);
    PARSE_INSTRUCTION_NOARG("nand", RAC0_NAND_OPCODE);
    PARSE_INSTRUCTION_NOARG("nor", RAC0_NOR_OPCODE);
    PARSE_INSTRUCTION_NOARG("xor", RAC0_XOR_OPCODE);

    // flow
    PARSE_INSTRUCTION_ARG("jmpa", RAC0_JMPA_OPCODE);
    PARSE_INSTRUCTION_NOARG("jmpt", RAC0_JMPT_OPCODE);
    PARSE_INSTRUCTION_NOARG("jza", RAC0_JZA_OPCODE);
    PARSE_INSTRUCTION_NOARG("jnza", RAC0_JNZA_OPCODE);
    PARSE_INSTRUCTION_NOARG("jnega", RAC0_JNEGA_OPCODE);
    PARSE_INSTRUCTION_NOARG("jposa", RAC0_JPOSA_OPCODE);

    // device
    PARSE_INSTRUCTION_ARG("setda", RAC0_SETDA_OPCODE);
    PARSE_INSTRUCTION_NOARG("setdt", RAC0_SETDT_OPCODE);
    PARSE_INSTRUCTION_ARG("fetchda", RAC0_FETCHDA_OPCODE);
    PARSE_INSTRUCTION_NOARG("fetchdt", RAC0_FETCHDT_OPCODE);
    PARSE_INSTRUCTION_ARG("putda", RAC0_PUTDA_OPCODE);
    PARSE_INSTRUCTION_NOARG("putdt", RAC0_PUTDT_OPCODE);

    // interrupt
    PARSE_INSTRUCTION_ARG("int", RAC0_INT_OPCODE);
    PARSE_INSTRUCTION_ARG("iret", RAC0_IRET_OPCODE);

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
        rac0a_free_token(label);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    string_t string;

    if(rac0a_parse_string(parser, &string).code != RAC0A_OK) {
        parser->lexer = backup;
        rac0a_free_token(label);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    value->label = label.lexeme;
    value->array = rac0a_string_copy(string);
    value->size = strlen(string);
    
    free(string);

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
        rac0a_free_token(label);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }
    
    if(rac0a_parse_value(parser, &value->value).code != RAC0A_OK) {
        parser->lexer = backup;
        rac0a_free_token(label);
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    value->label = rac0a_string_copy(label.lexeme);
    rac0a_free_token(label);

    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_statement_list(rac0a_parser_t* parser, rac0a_hl_statement_list_t* list) {
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
        } else if(rac0a_parse_eof(parser).code == RAC0A_OK) {
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

// todo
rac0a_parse_result_t rac0a_parse_constblock_usage(rac0a_parser_t* parser);

// todo
rac0a_parse_result_t rac0a_parse_constblock_argument_usage(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_constblock_usage(parser).code == RAC0A_OK) {
        return (rac0a_parse_result_t) { RAC0A_OK };
    }

    rac0a_hl_value_t value;
    if(rac0a_parse_value(parser, &value).code == RAC0A_OK)
        return (rac0a_parse_result_t) { RAC0A_OK };

    return (rac0a_parse_result_t) { RAC0A_ERROR };
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_argument_list_usage(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    while (1) {
        if(rac0a_parse_constblock_argument_usage(parser).code == RAC0A_OK) {
            if(rac0a_parse_comma(parser).code == RAC0A_OK) 
                continue;
        }

        if (rac0a_parse_r_paren(parser).code == RAC0A_OK)
            break;

        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    return (rac0a_parse_result_t) { RAC0A_OK };
}

// todo
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

    if(rac0a_parse_constblock_argument_list_usage(parser).code != RAC0A_OK) {
        parser->lexer = backup;
        return (rac0a_parse_result_t) { RAC0A_ERROR };
    }

    // already handled by rac0a_parse_constblock_argument_list_usage
    // if(rac0a_parse_r_paren(parser).code != RAC0A_OK) {
    //     parser->lexer = backup;
    //     return (rac0a_parse_result_t) { RAC0A_ERROR };
    // }

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

rac0a_hl_statement_list_t rac0a_parse_program(const string_t input) {
    rac0a_parser_t parser = {
        .lexer = (rac0a_lexer_t) {
            .input = input,
            .pointer = 0
        }
    };

    create_vector(&parser.hl_statements, 1024);

    while(1) {
        if(rac0a_parse_eof(&parser).code == RAC0A_OK)
            break;

        if(rac0a_parse_statement_list(&parser, &parser.hl_statements).code == RAC0A_OK) {
            PLUM_LOG(PLUM_TRACE, "Statement list");
            continue;
        }
            
        PLUM_LOG(PLUM_ERROR, "Dead end");
        break;
    }

    return parser.hl_statements;
}
