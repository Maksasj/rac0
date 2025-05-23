#include "rac0a_parser.h"

rac0a_parse_result_t rac0a_parse_result_ok() {
    return (rac0a_parse_result_t) { RAC0A_OK };
}

rac0a_parse_result_t rac0a_parse_result_failed(const char* message, rac0_u64_t pointer) {
    return (rac0a_parse_result_t) { 
        .code = RAC0A_FAILED, 
        .as.error = {
            .message = message,
            .pointer = pointer,
        }
    };
}

rac0a_parse_result_t rac0a_parse_result_error(const char* message, rac0_u64_t pointer) {
    return (rac0a_parse_result_t) { 
        .code = RAC0A_ERROR, 
        .as.error = {
            .message = message,
            .pointer = pointer,
        }
    };
}

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
    
            return rac0a_parse_result_ok();
        }

        break;
    }

    parser->lexer = backup;
    
    return rac0a_parse_result_error("todo", parser->lexer.pointer);
}

// todo
void rac0a_parse_look_next_token(rac0a_parser_t* parser, rac0a_token_t* ret) {
    rac0a_lexer_t backup = parser->lexer;
    
    // there we skip comments
    while (1) {
        rac0a_token_t token = rac0a_next_token(&parser->lexer);
        
        if(token.type == RAC0A_TOKEN_COMMENT)
            continue;
        
        if(ret != NULL)
            *ret = token;
        else
            rac0a_free_token(token);

        break;
    }

    parser->lexer = backup;
}

rac0a_parse_result_t rac0a_parse_exact_word(rac0a_parser_t* parser, const string_t lexem) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Expected label token", parser->lexer.pointer);
    }
        
    if(strcmp(token.lexeme, lexem) != 0) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return rac0a_parse_result_error("Expected label lexeme to match", parser->lexer.pointer);
    }

    rac0a_free_token(token);

    return rac0a_parse_result_ok();
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
        if(strncmp(token.lexeme, "0b", 2) == 0) {
            *number = strtoull(token.lexeme + 2, NULL, 2);
        } else if(strncmp(token.lexeme, "0x", 2) == 0) {
            *number = strtoull(token.lexeme, NULL, 0);
        } else if(strncmp(token.lexeme, "'", 1) == 0) {
            *number = token.lexeme[1];
        } else {
            rac0a_free_token(token);
            return rac0a_parse_result_error("Failed to parse number not suported number format", parser->lexer.pointer);
        }

        rac0a_free_token(token);
        return rac0a_parse_result_ok();
    }

    return rac0a_parse_result_error("Failed to parse number", parser->lexer.pointer);
}

rac0a_parse_result_t rac0a_parse_l_bracket(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_L_BRACKET, NULL);
}

rac0a_parse_result_t rac0a_parse_r_bracket(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_R_BRACKET, NULL);
}

rac0a_parse_result_t rac0a_parse_l_squarebracket(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_L_SQUARE_BRACKET, NULL);
}

rac0a_parse_result_t rac0a_parse_r_squarebracket(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_R_SQUARE_BRACKET, NULL);
}

rac0a_parse_result_t rac0a_parse_percent(rac0a_parser_t* parser) {
    return rac0a_parse_token(parser, RAC0A_TOKEN_PERCENT, NULL);
}

rac0a_parse_result_t rac0a_parse_string(rac0a_parser_t* parser, string_t* string) {
    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_STRING, &token).code == RAC0A_OK) {
        *string = rac0a_string_copy(token.lexeme);
        rac0a_free_token(token);
        return rac0a_parse_result_ok();
    }

    return rac0a_parse_result_error("Failed to parse string", parser->lexer.pointer);
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

    rac0a_parse_result_t result;

    result = rac0a_parse_at(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse include statement, expected @ symbol", parser->lexer.pointer);
    }

    result = rac0a_parse_exact_word(parser, "include");
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse include statement, expected 'include' keyword", parser->lexer.pointer);
    }

    string_t include_path;
    result = rac0a_parse_string(parser, &include_path);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {
            
    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_failed("Failed to parse include statement, expected string", parser->lexer.pointer);
    }

    string_t source = rac0_utils_read_file_string(include_path);

    if(source == NULL) {
        parser->lexer = backup;
        free(include_path);
        return rac0a_parse_result_failed("No such file or directory", parser->lexer.pointer);
    }

    // Parse program
    rac0a_parser_t child_parser = {
        .lexer = (rac0a_lexer_t) {
            .input = source,
            .pointer = 0
        }
    };
    create_vector(&child_parser.hl_statements, 1024);

    rac0a_parse_result_t parse_result = rac0a_parse_program(&child_parser);

    if(parse_result.code == RAC0A_OK) {

    } else if(parse_result.code == RAC0A_FAILED) {
        rac0_value_t pointer = parse_result.as.error.pointer;

        rac0_value_t token_start = pointer;
        while (isspace(source[token_start]))
            token_start++;

        rac0_value_t line = rac0a_get_line_number_from_index(source, pointer) + 2;
        rac0_value_t column = 0;
        rac0_value_t line_length = rac0a_get_line_length(source + token_start); 

        printf("%s:%llu:%llu: \x1B[31merror\x1B[0m: %s\n", include_path, line, column, parse_result.as.error.message);

        printf("   %llu | \x1B[31m", line);

        for(int i = 0; i < line_length; ++i)
            printf("%c", source[token_start + i]);

        printf("\x1B[0m\n");

        printf("   %llu | \x1B[31m^", line + 1);

        for(int i = 0; i < line_length - 1; ++i)
            printf("~");

        printf("\x1B[0m\n");
        
        free(include_path);
        return rac0a_parse_result_failed("Included from file", backup.pointer);
    } else if(parse_result.code == RAC0A_ERROR) {

    }

    for(int i = 0; i < vector_size(&child_parser.hl_statements); ++i)
        vector_push(&parser->hl_statements, vector_get(&child_parser.hl_statements, i)); 

    free_vector(&child_parser.hl_statements); // we dont want to free child hl statements

    free(include_path);
    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_constval_definition(rac0a_parser_t* parser, rac0a_hl_constval_statement_t* constval) {
    rac0a_lexer_t backup = parser->lexer;

    // parse constval declaration statement
    if(rac0a_parse_at(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constval statement, expected @ symbol", parser->lexer.pointer);
    }

    if(rac0a_parse_exact_word(parser, "constval").code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constval statement, expected 'constval' keyword", parser->lexer.pointer);
    }

    // parse constval declaration 
    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_failed("Failed to parse constval statement, expected label", parser->lexer.pointer);
    }

    rac0_value_t value;
    if(rac0a_parse_number(parser, &value).code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return rac0a_parse_result_failed("Failed to parse constval statement, expected number", parser->lexer.pointer);
    }

    constval->label = rac0a_string_copy(token.lexeme);
    constval->value = value;

    rac0a_free_token(token);

    return rac0a_parse_result_ok();
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_constval_argument_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_at(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition constval argument, expected '@' symbol", parser->lexer.pointer);
    }

    if(rac0a_parse_exact_word(parser, "constval").code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition constval argument, expected 'constval' keyword", parser->lexer.pointer);
    }

    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition constval argument, expected label", parser->lexer.pointer);
    }

    rac0a_free_token(token);

    return rac0a_parse_result_ok();   
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_constblock_argument_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_at(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition constblock argument, expected '@' symbol", parser->lexer.pointer);
    }

    if(rac0a_parse_exact_word(parser, "constblock").code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition constblock argument, expected 'constblock' keyword", parser->lexer.pointer);
    }

    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition constblock argument, expected label", parser->lexer.pointer);
    }

    rac0a_free_token(token);

    return rac0a_parse_result_ok();   
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_argument_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_constblock_constval_argument_definition(parser).code == RAC0A_OK) {
        return rac0a_parse_result_ok();  
    }
        
    if(rac0a_parse_constblock_constblock_argument_definition(parser).code == RAC0A_OK) {
        return rac0a_parse_result_ok();  
    }

    parser->lexer = backup;
    return rac0a_parse_result_error("Failed to parse constblock definition argument", parser->lexer.pointer);
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
        return rac0a_parse_result_error("Failed to parse constblock definition argument list", parser->lexer.pointer);
    }

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_constblock_definition(rac0a_parser_t* parser, rac0a_hl_constblock_statement_t* block) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_parse_result_t result;

    // parse constval declaration statement
    result = rac0a_parse_at(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition, expected '@' symbol", parser->lexer.pointer);
    }

    result = rac0a_parse_exact_word(parser, "constblock");
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition, expected 'constblock' keyword", parser->lexer.pointer);
    }

    // parse constval declaration 


    rac0a_token_t token;
    result = rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock definition, expected label", parser->lexer.pointer);
    }

    result = rac0a_parse_l_paren(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return rac0a_parse_result_error("Failed to parse constblock definition, expected '(' symbol", parser->lexer.pointer);
    }
    
    result = rac0a_parse_constblock_argument_list_definition(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return rac0a_parse_result_error("Failed to parse constblock definition, failed to parse constblock argument list", parser->lexer.pointer);
    }

    // already handled by rac0a_parse_constblock_argument_list_definition
    // if(rac0a_parse_r_paren(parser).code == RAC0A_ERROR) {
    //     parser->lexer = backup;
    //     rac0a_free_token(token);
    //     return rac0a_parse_result_error("todo", parser->lexer.pointer);
    // }
    
    result = rac0a_parse_l_bracket(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return rac0a_parse_result_error("Failed to parse constblock definition, expected '{' symbol", parser->lexer.pointer);
    }
    
    rac0a_hl_statement_list_t constblock_statements;
    create_vector(&constblock_statements, 100);

    result = rac0a_parse_statement_list(parser, &constblock_statements);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(token);
        free_vector(&constblock_statements);
        return rac0a_parse_result_error(result.as.error.message, parser->lexer.pointer);
    }

    result = rac0a_parse_r_bracket(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } else if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(token);
        rac0a_free_hl_statement_list(&constblock_statements);
        return rac0a_parse_result_error("Failed to parse constblock definition, expected '}' symbol", parser->lexer.pointer);
    }

    block->label = rac0a_string_copy(token.lexeme);
    block->statements = constblock_statements;

    rac0a_free_token(token);

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_label_definition(rac0a_parser_t* parser, rac0a_hl_label_statement_t* label) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t token;

    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse label definition, expected label", parser->lexer.pointer);
    }
    
    if(rac0a_parse_colon(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(token);
        return rac0a_parse_result_error("Failed to parse label definition, expected ':' symbol", parser->lexer.pointer);
    }

    label->label = rac0a_string_copy(token.lexeme);
    rac0a_free_token(token);

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_const_thing_usage(rac0a_parser_t* parser, string_t* label) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_dollar(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse const thing usage, expected '$' symbol", parser->lexer.pointer);
    }

    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse const thing usage, expected label", parser->lexer.pointer);
    }

    *label = rac0a_string_copy(token.lexeme);
    rac0a_free_token(token);

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_label_pointer(rac0a_parser_t* parser, string_t* label) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_ampersand(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse label pointer usage, expected '&'", parser->lexer.pointer);
    }

    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse label pointer usage, expected label", parser->lexer.pointer);
    }

    *label = rac0a_string_copy(token.lexeme);
    rac0a_free_token(token);

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_value(rac0a_parser_t* parser, rac0a_hl_value_t* value) {
    rac0_value_t number;
    string_t label;

    if(rac0a_parse_number(parser, &number).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_NUMBER;
        value->as.value = number;

        return rac0a_parse_result_ok();
    }

    if(rac0a_parse_const_thing_usage(parser, &label).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_CONSTVAL;
        value->as.constval_label = rac0a_string_copy(label);

        return rac0a_parse_result_ok();
    }

    if(rac0a_parse_label_pointer(parser, &label).code == RAC0A_OK) {
        value->type = RAC0A_HL_VALUE_TYPE_LABEL_POINTER;
        value->as.label = label = rac0a_string_copy(label);

        return rac0a_parse_result_ok();
    }

    return rac0a_parse_result_error("Failed to parse value reference", parser->lexer.pointer);
}

rac0a_parse_result_t rac0a_parse_instruction_noarg(rac0a_parser_t* parser, const string_t lexem) {
    return rac0a_parse_exact_word(parser, lexem);
}

rac0a_parse_result_t rac0a_parse_instruction_arg(rac0a_parser_t* parser, const string_t lexem, rac0a_hl_value_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_parse_result_t result;
    result = rac0a_parse_exact_word(parser, lexem);

    if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse instruction with arguments, expected instruction identifier", parser->lexer.pointer);
    }

    result = rac0a_parse_value(parser, value);
    if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_failed("Failed to parse instruction with arguments, expected value", parser->lexer.pointer);
    }

    return rac0a_parse_result_ok();
}

#define PARSE_INSTRUCTION_NOARG(label, operation_code)                              \
    result = rac0a_parse_instruction_noarg(parser, label);                          \
    if(result.code == RAC0A_OK) {                                                   \
        /* PLUM_LOG(PLUM_TRACE, label " instruction definition"); */                \
                                                                                    \
        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;                         \
        inst->inst.as.opcode = operation_code;                                      \
        inst->value.type = RAC0A_HL_VALUE_TYPE_NONE;                                \
                                                                                    \
        return rac0a_parse_result_ok();                                             \
    } else if(result.code == RAC0A_FAILED) {                                        \
        /* continue */                                                              \
    } else if(result.code == RAC0A_ERROR) {                                         \
        /* continue */                                                              \
    }                                                                               \

#define PARSE_INSTRUCTION_ARG(label, operation_code)                                \
    result =  rac0a_parse_instruction_arg(parser, label, &value);                   \
    if(result.code == RAC0A_OK) {                                                   \
        /* PLUM_LOG(PLUM_TRACE, label " instruction definition"); */                \
                                                                                    \
        inst->inst.type = RAC0A_HL_INSTRUCTION_TYPE_OPCODE;                         \
        inst->inst.as.opcode = operation_code;                                      \
        inst->value = value;                                                        \
                                                                                    \
        return rac0a_parse_result_ok();                                             \
    } else if(result.code == RAC0A_FAILED) {                                        \
        return result;                                                              \
    } else if(result.code == RAC0A_ERROR) {                                         \
        /* continue */                                                              \
    }                                                                               \

rac0a_parse_result_t rac0a_parse_instruction(rac0a_parser_t* parser, rac0a_hl_instruction_statement_t* inst) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_hl_value_t value;
    rac0a_parse_result_t result;

    // cpu
    PARSE_INSTRUCTION_NOARG("halt", RAC0_HALT_OPCODE);
    PARSE_INSTRUCTION_NOARG("wait", RAC0_WAIT_OPCODE);
    PARSE_INSTRUCTION_NOARG("setidtt", RAC0_SETIDTT_OPCODE);
    PARSE_INSTRUCTION_NOARG("setidtst", RAC0_SETIDTST_OPCODE);
    PARSE_INSTRUCTION_NOARG("setptbat", RAC0_SETPTBAT_OPCODE);
    PARSE_INSTRUCTION_NOARG("setptst", RAC0_SETPTST_OPCODE);
    PARSE_INSTRUCTION_NOARG("setptpst", RAC0_SETPTPST_OPCODE);
    PARSE_INSTRUCTION_NOARG("settt", RAC0_SETTT_OPCODE);
    PARSE_INSTRUCTION_ARG("settta", RAC0_SETTTA_OPCODE);
    PARSE_INSTRUCTION_NOARG("setirett", RAC0_SETIRETT_OPCODE);
    PARSE_INSTRUCTION_NOARG("setstt", RAC0_SETSTT_OPCODE);

    // stack
    PARSE_INSTRUCTION_ARG("pusha", RAC0_PUSHA_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushpc", RAC0_PUSHPC_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushss", RAC0_PUSHSS_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushdc", RAC0_PUSHDC_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushms", RAC0_PUSHMS_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushiret", RAC0_PUSHIRET_OPCODE);
    PARSE_INSTRUCTION_NOARG("dupt", RAC0_DUPT_OPCODE);
    PARSE_INSTRUCTION_NOARG("dupn", RAC0_DUPN_OPCODE);
    PARSE_INSTRUCTION_NOARG("drop", RAC0_DROP_OPCODE);
    PARSE_INSTRUCTION_NOARG("swap", RAC0_SWAP_OPCODE);

    // memory
    PARSE_INSTRUCTION_NOARG("store", RAC0_STORE_OPCODE);
    PARSE_INSTRUCTION_ARG("storearac", RAC0_STOREARAC_OPCODE);
    PARSE_INSTRUCTION_ARG("storea", RAC0_STOREA_OPCODE);
    PARSE_INSTRUCTION_NOARG("load", RAC0_LOAD_OPCODE);
    PARSE_INSTRUCTION_ARG("loadarac", RAC0_LOADARAC_OPCODE);
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
    PARSE_INSTRUCTION_NOARG("jmptc", RAC0_JMPTC_OPCODE);
    PARSE_INSTRUCTION_ARG("jza", RAC0_JZA_OPCODE);
    PARSE_INSTRUCTION_ARG("jnza", RAC0_JNZA_OPCODE);
    PARSE_INSTRUCTION_ARG("jnega", RAC0_JNEGA_OPCODE);
    PARSE_INSTRUCTION_ARG("jposa", RAC0_JPOSA_OPCODE);

    // device
    PARSE_INSTRUCTION_ARG("setda", RAC0_SETDA_OPCODE);
    PARSE_INSTRUCTION_NOARG("setdt", RAC0_SETDT_OPCODE);
    PARSE_INSTRUCTION_ARG("fetchda", RAC0_FETCHDA_OPCODE);
    PARSE_INSTRUCTION_NOARG("fetchdt", RAC0_FETCHDT_OPCODE);
    PARSE_INSTRUCTION_ARG("putda", RAC0_PUTDA_OPCODE);
    PARSE_INSTRUCTION_NOARG("putdt", RAC0_PUTDT_OPCODE);

    // interrupt
    PARSE_INSTRUCTION_ARG("int", RAC0_INT_OPCODE);
    PARSE_INSTRUCTION_ARG("iretac", RAC0_IRETAC_OPCODE);

    // extra
    PARSE_INSTRUCTION_ARG("setstbta", RAC0_SETSTBTA_OPCODE);
    PARSE_INSTRUCTION_ARG("setstbfa", RAC0_SETSTBFA_OPCODE);
    PARSE_INSTRUCTION_ARG("loadsta", RAC0_LOADSTA_OPCODE);
    PARSE_INSTRUCTION_ARG("storesta", RAC0_STORESTA_OPCODE);
    PARSE_INSTRUCTION_NOARG("iretcc", RAC0_IRETCC_OPCODE);
    PARSE_INSTRUCTION_ARG("loop", RAC0_LOOP_OPCODE);
    PARSE_INSTRUCTION_ARG("mcpyba", RAC0_MCPYBA_OPCODE);
    PARSE_INSTRUCTION_NOARG("setstssc", RAC0_SETSTSSC_OPCODE);
    PARSE_INSTRUCTION_ARG("storessa", RAC0_STORESSA_OPCODE);
    PARSE_INSTRUCTION_ARG("loadssa", RAC0_LOADSSA_OPCODE);
    PARSE_INSTRUCTION_NOARG("clearst", RAC0_CLEARST_OPCODE);
    PARSE_INSTRUCTION_NOARG("dropiret", RAC0_DROPIRET_OPCODE);
    PARSE_INSTRUCTION_NOARG("pushiretc", RAC0_PUSHIRETC_OPCODE);
    PARSE_INSTRUCTION_ARG("modac", RAC0_MODAC_OPCODE);
    PARSE_INSTRUCTION_ARG("addac", RAC0_ADDAC_OPCODE);
    PARSE_INSTRUCTION_ARG("mulac", RAC0_MULAC_OPCODE);

    parser->lexer = backup;

    return rac0a_parse_result_error("Failed to parse instruction, unknown instruction", parser->lexer.pointer);
}

// TODO
rac0a_parse_result_t rac0a_parse_array_size_definition(rac0a_parser_t* parser, rac0_value_t* number) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_l_squarebracket(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse empty byte array definition, expected '[' symbol", parser->lexer.pointer);
    }

    if(rac0a_parse_number(parser, number).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse empty byte array definition, expected number", parser->lexer.pointer);
    }

    if(rac0a_parse_r_squarebracket(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse empty byte array definition, expected ']' symbol",  parser->lexer.pointer);
    }
    
    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_empty_array_definition(rac0a_parser_t* parser, rac0a_hl_byte_def_statement_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    rac0_value_t final_number = 0;

    if(rac0a_parse_array_size_definition(parser, &final_number).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse array size", parser->lexer.pointer);
    }

    while(1) {
        rac0_value_t number = 0;

        if(rac0a_parse_array_size_definition(parser, &number).code == RAC0A_ERROR)
            break;

        final_number *= number;
    }

    value->array = calloc(final_number, 1);
    value->size = final_number;

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_byte_definition_value(rac0a_parser_t* parser, rac0a_hl_byte_def_statement_t* value) {
    string_t string;

    if(rac0a_parse_string(parser, &string).code == RAC0A_OK) {
        value->array = rac0a_string_copy(string);
        value->size = strlen(string);
        
        free(string);

        return rac0a_parse_result_ok();
    }

    if(rac0a_parse_empty_array_definition(parser, value).code == RAC0A_OK) {
        return rac0a_parse_result_ok();
    }

    return rac0a_parse_result_error("Failed to parse byte definition", parser->lexer.pointer);
}

rac0a_parse_result_t rac0a_parse_byte_definition(rac0a_parser_t* parser, rac0a_hl_byte_def_statement_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t label;
    rac0a_parse_result_t result;

    result = rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &label);
    if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse byte definition, expected label", parser->lexer.pointer);
    }

    result = rac0a_parse_exact_word(parser, "db");
    if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(label);
        return rac0a_parse_result_error("Failed to parse byte definition, expected 'db' keyword", parser->lexer.pointer);
    }

    result = rac0a_parse_byte_definition_value(parser, value);
    if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(label);
        return rac0a_parse_result_failed("Failed to parse byte definition", parser->lexer.pointer);
    }

    value->label = label.lexeme;

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_word_definition(rac0a_parser_t* parser, rac0a_hl_word_def_statement_t* value) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_token_t label;
    rac0a_parse_result_t result;

    result = rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &label);
    if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse word definition, expected label", parser->lexer.pointer);
    }

    result = rac0a_parse_exact_word(parser, "dw");
    if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(label);
        return rac0a_parse_result_error("Failed to parse word definition, expected 'dw' keyword", parser->lexer.pointer);
    }
    
    result =  rac0a_parse_value(parser, &value->value);
    if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        rac0a_free_token(label);
        return rac0a_parse_result_failed("Failed to parse word definition, expected value", parser->lexer.pointer);
    }

    value->label = rac0a_string_copy(label.lexeme);
    rac0a_free_token(label);

    return rac0a_parse_result_ok();
}

// todo
// rac0a_parse_result_t rac0a_parse_statement_starts_with_label(rac0a_parser_t* parser, rac0a_hl_statement_list_t* list) {
// 
// }
// 
// rac0a_parse_result_t rac0a_parse_statement_starts_with_at(rac0a_parser_t* parser, rac0a_hl_statement_list_t* list) {
// 
// }
// 
// rac0a_parse_result_t rac0a_parse_statement_starts_with_dollar(rac0a_parser_t* parser, rac0a_hl_statement_list_t* list) {
// 
// }

rac0a_parse_result_t rac0a_parse_statement(rac0a_parser_t* parser, rac0a_hl_statement_list_t* list) {
    rac0a_hl_constblock_statement_t constblock;
    rac0a_hl_constval_statement_t constval;
    rac0a_hl_instruction_statement_t inst;
    rac0a_hl_label_statement_t label;
    rac0a_hl_word_def_statement_t word_def;
    rac0a_hl_byte_def_statement_t byte_def;

    rac0a_parse_result_t result;

    rac0a_token_t first_token;
    rac0a_parse_look_next_token(parser, &first_token);

    rac0_value_t pointer = parser->lexer.pointer;

    if(first_token.type == RAC0A_TOKEN_LABEL) {
        result = rac0a_parse_byte_definition(parser, &byte_def);
        if(result.code == RAC0A_OK) {
            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->pointer = pointer;
            st->type = RAC0A_HL_TYPE_BYTE_DEF;
            st->as.byte_def = byte_def;
            vector_push(list, st);
            rac0a_free_token(first_token);
            return rac0a_parse_result_ok();
        } else if(result.code == RAC0A_FAILED) {
            return result;
        } else if(result.code == RAC0A_ERROR) {
            // continue
        }
    
        result = rac0a_parse_word_definition(parser, &word_def);
        if(result.code == RAC0A_OK) {
            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->pointer = pointer;
            st->type = RAC0A_HL_TYPE_WORD_DEF;
            st->as.word_def = word_def;
            vector_push(list, st);
            rac0a_free_token(first_token);
            return rac0a_parse_result_ok();
        } else if(result.code == RAC0A_FAILED) {
            return result;
        } else if(result.code == RAC0A_ERROR) {
            // continue
        }
    
        result = rac0a_parse_label_definition(parser, &label);
        if(result.code == RAC0A_OK) {
            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->pointer = pointer;
            st->type = RAC0A_HL_TYPE_LABEL;
            st->as.label = label;
            vector_push(list, st);
            rac0a_free_token(first_token);
            return rac0a_parse_result_ok();
        } else if(result.code == RAC0A_FAILED) {
            return result;
        } else if(result.code == RAC0A_ERROR) {
            // continue
        }
            
        result = rac0a_parse_instruction(parser, &inst);
        if(result.code == RAC0A_OK) {
            rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
            st->pointer = pointer;
            st->type = RAC0A_HL_TYPE_INSTRUCTION;
            st->as.instruction = inst;
            vector_push(list, st);
            rac0a_free_token(first_token);
            return rac0a_parse_result_ok();
        } else if(result.code == RAC0A_FAILED) {
            return result;
        } else if(result.code == RAC0A_ERROR) {
            // continue
        }

        rac0a_free_token(first_token);
        return rac0a_parse_result_failed("Unknown instruction", pointer);
    }
    
    rac0a_free_token(first_token);

    result = rac0a_parse_include_statement(parser);
    if(result.code == RAC0A_OK) {
        return rac0a_parse_result_ok();
    } else if(result.code == RAC0A_FAILED) {
        return result;
    } else if(result.code == RAC0A_ERROR) {
        // continue
    }

    result = rac0a_parse_constval_definition(parser, &constval);
    if(result.code == RAC0A_OK) {
        rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
        st->pointer = pointer;
        st->type = RAC0A_HL_TYPE_CONSTVAL_DECL;
        st->as.constval = constval;
        vector_push(list, st);
        return rac0a_parse_result_ok();
    } else if(result.code == RAC0A_FAILED) {
        return result;
    } else if(result.code == RAC0A_ERROR) {
        // continue
    }

    result = rac0a_parse_constblock_definition(parser, &constblock);
    if(result.code == RAC0A_OK) {
        rac0a_hl_statement_t* st = (rac0a_hl_statement_t*) malloc(sizeof(rac0a_hl_statement_t));
        st->pointer = pointer;
        st->type = RAC0A_HL_TYPE_CONSTBLOCK_DECL;
        st->as.constblock = constblock;
        vector_push(&parser->hl_statements, st);
        return rac0a_parse_result_ok();
    } else if(result.code == RAC0A_FAILED) {
        return result;
    } else if(result.code == RAC0A_ERROR) {
        // continue
    }

    result = rac0a_parse_module_definition(parser);
    if(result.code == RAC0A_OK) {
        return rac0a_parse_result_ok();
    } else if(result.code == RAC0A_FAILED) {
        return result;
    } else if(result.code == RAC0A_ERROR) {
        // continue
    }

    result = rac0a_parse_constblock_usage(parser);
    if(result.code == RAC0A_OK) {
        return rac0a_parse_result_ok();
    } else if(result.code == RAC0A_FAILED) {
        return result;
    } else if(result.code == RAC0A_ERROR) {
        // continue
    }
    
    return rac0a_parse_result_error("Failed to parse statement, unknown statement", parser->lexer.pointer);
}

rac0a_parse_result_t rac0a_parse_statement_list(rac0a_parser_t* parser, rac0a_hl_statement_list_t* list) {
    while(1) {

        rac0a_parse_result_t result;
        
        result = rac0a_parse_statement(parser, list);
        if(result.code == RAC0A_OK) {
            continue;
        } else if(result.code == RAC0A_FAILED) {
            return result;
        } else if(result.code == RAC0A_ERROR) {
            // return result;
            break;
        }   

        /*
        result = rac0a_parse_eof(parser);
        if(result.code == RAC0A_OK) {
            --parser->lexer.pointer;
            break;
        } else if(result.code == RAC0A_FAILED) {
            return rac0a_parse_result_error("Expected end of file", 0);
        } else if(result.code == RAC0A_ERROR) {
            return rac0a_parse_result_error("Expected end of file", 0);
        }   
        
        result = rac0a_parse_r_bracket(parser);
        if(result.code == RAC0A_OK) {
            --parser->lexer.pointer;
            break;
        } else if(result.code == RAC0A_FAILED) {
            return rac0a_parse_result_error("Expected '}'", 0);
        } else if(result.code == RAC0A_ERROR) {
            return rac0a_parse_result_error("Expected '}'", 0);
        }   

             PLUM_UNREACHABLE();
        */
    }

    return rac0a_parse_result_ok();
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_usage(rac0a_parser_t* parser);

// todo
rac0a_parse_result_t rac0a_parse_constblock_argument_usage(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_constblock_usage(parser).code == RAC0A_OK)
        return rac0a_parse_result_ok();

    rac0a_hl_value_t value;
    if(rac0a_parse_value(parser, &value).code == RAC0A_OK)
        return rac0a_parse_result_ok();

    return rac0a_parse_result_error("Failed to parse constblock usage argument", parser->lexer.pointer);
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_argument_list_usage(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    while (1) {
        if(rac0a_parse_constblock_argument_usage(parser).code == RAC0A_OK)
            if(rac0a_parse_comma(parser).code == RAC0A_OK) 
                continue;

        if (rac0a_parse_r_paren(parser).code == RAC0A_OK)
            break;

        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock argument list", parser->lexer.pointer);
    }

    return rac0a_parse_result_ok();
}

// todo
rac0a_parse_result_t rac0a_parse_constblock_usage(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    if(rac0a_parse_dollar(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock usage, expected '$' symbol", parser->lexer.pointer);
    }

    if(rac0a_parse_label(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock usage, expected label", parser->lexer.pointer);
    }

    if(rac0a_parse_l_paren(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock usage, expected '(' symbol", parser->lexer.pointer);
    }

    if(rac0a_parse_constblock_argument_list_usage(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse constblock usage, failed to parse argument list", parser->lexer.pointer);
    }

    // already handled by rac0a_parse_constblock_argument_list_usage
    // if(rac0a_parse_r_paren(parser).code == RAC0A_ERROR) {
    //     parser->lexer = backup;
    //     return rac0a_parse_result_error("todo", parser->lexer.pointer);
    // }

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_module_definition(rac0a_parser_t* parser) {
    rac0a_lexer_t backup = parser->lexer;

    rac0a_parse_result_t result;

    result = rac0a_parse_at(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse module definition, expected '@' symbol", parser->lexer.pointer);
    }

    result = rac0a_parse_exact_word(parser, "module");
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse module definition, expected 'module' keyword", parser->lexer.pointer);
    }

    result = rac0a_parse_label(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse module definition, expected label", parser->lexer.pointer);
    }

    /* todo
    rac0a_token_t token;
    if(rac0a_parse_token(parser, RAC0A_TOKEN_LABEL, &token).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse label pointer usage, expected label", parser->lexer.pointer);
    }

    *label = rac0a_string_copy(token.lexeme);
    rac0a_free_token(token);
    */

    // there we should validate if module was prev included

    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } if(rac0a_parse_l_bracket(parser).code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse module definition, expected '{' symbol", parser->lexer.pointer);
    }
    
    result = rac0a_parse_statement_list(parser, &parser->hl_statements);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error(result.as.error.message, parser->lexer.pointer);
    }

    result = rac0a_parse_r_bracket(parser);
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {

    } if(result.code == RAC0A_ERROR) {
        parser->lexer = backup;
        return rac0a_parse_result_error("Failed to parse module definition, expected '}' symbol", parser->lexer.pointer);
    }

    return rac0a_parse_result_ok();
}

rac0a_parse_result_t rac0a_parse_program(rac0a_parser_t* parser) {
    rac0a_parse_result_t result;
    result = rac0a_parse_statement_list(parser, &parser->hl_statements);
        
    if(result.code == RAC0A_OK) {

    } else if(result.code == RAC0A_FAILED) {
        return result;
    } if(result.code == RAC0A_ERROR) {

    }

    result = rac0a_parse_eof(parser);
    if(result.code == RAC0A_OK) {

    }

    return rac0a_parse_result_ok();
}
