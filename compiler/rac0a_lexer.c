#include "rac0a_lexer.h"

void rac0a_free_token(rac0a_token_t token) {
    if(token.lexeme != NULL)
        free(token.lexeme);
}

void rac0a_skip_whitespace(rac0a_lexer_t* lexer) {
    while (isspace(lexer->input[lexer->pointer]))
        lexer->pointer++;
}

rac0a_lex_result_t rac0a_lex_at(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != '@')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_AT;
    token->lexeme = rac0a_string_copy("@");

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_dollar(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != '$')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_DOLLAR;
    token->lexeme = rac0a_string_copy("$");

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_lparen(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != '(')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_L_PAREN;
    token->lexeme = rac0a_string_copy("(");

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_rparen(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != ')')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_R_PAREN;
    token->lexeme = rac0a_string_copy(")");

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_colon(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != ':')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_COLON;
    token->lexeme = rac0a_string_copy(":");

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_label(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    int start = lexer->pointer;

    if(!isalpha(lexer->input[lexer->pointer]) && lexer->input[lexer->pointer] != '_')
        goto cont;

    ++lexer->pointer;

    while(1) {
        if(!isalpha(lexer->input[lexer->pointer]) && lexer->input[lexer->pointer] != '_' && !isdigit(lexer->input[lexer->pointer]))
            break;

        ++lexer->pointer;
    }

    cont:

    if(start == lexer->pointer) {
        lexer->pointer = start;
        return (rac0a_lex_result_t) { RAC0A_ERROR };
    }

    token->type = RAC0A_TOKEN_LABEL;
    token->lexeme = rac0a_string_copy_len(&lexer->input[start], (lexer->pointer - 1) - start);

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_hex_number(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(strncmp(lexer->input + lexer->pointer, "0x", 2) != 0)
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    int start = lexer->pointer;
    lexer->pointer += 2;

    while(1) {
        if(!isxdigit(lexer->input[lexer->pointer]))
            break;

        ++lexer->pointer;
    }

    if((start + 2) == lexer->pointer) {
        lexer->pointer = start;
        return (rac0a_lex_result_t) { RAC0A_ERROR };
    }

    token->type = RAC0A_TOKEN_NUMBER;
    token->lexeme = rac0a_string_copy_len(&lexer->input[start], (lexer->pointer - 1) - start);

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_binary_number(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(strncmp(lexer->input + lexer->pointer, "0b", 2) != 0)
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    int start = lexer->pointer;
    lexer->pointer += 2;

    while(1) {
        if(lexer->input[lexer->pointer] != '0' && lexer->input[lexer->pointer] != '1')
            break;

        ++lexer->pointer;
    }

    if((start + 2) == lexer->pointer) {
        lexer->pointer = start;
        return (rac0a_lex_result_t) { RAC0A_ERROR };
    }

    token->type = RAC0A_TOKEN_NUMBER;
    token->lexeme = rac0a_string_copy_len(&lexer->input[start], (lexer->pointer - 1) - start);

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_number(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(rac0a_lex_hex_number(token, lexer).code == RAC0A_OK)
        return (rac0a_lex_result_t) { RAC0A_OK };

    if(rac0a_lex_binary_number(token, lexer).code == RAC0A_OK)
        return (rac0a_lex_result_t) { RAC0A_OK };

    if(rac0a_lex_character(token, lexer).code == RAC0A_OK)
        return (rac0a_lex_result_t) { RAC0A_OK };

    return (rac0a_lex_result_t) { RAC0A_ERROR };
}

rac0a_lex_result_t rac0a_lex_eof(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != '\0')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_EOF;
    token->lexeme = rac0a_string_copy("EOF");;

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_lbracket(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != '{')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_L_BRACKET;
    token->lexeme = rac0a_string_copy("{");

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_rbracket(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != '}')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_R_BRACKET;
    token->lexeme = rac0a_string_copy("}");;

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_percent(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != '%')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_R_BRACKET;
    token->lexeme = rac0a_string_copy("%");;

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_ampersand(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != '&')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_AMPERSAND;
    token->lexeme = rac0a_string_copy("&");;

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_comment(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(strncmp(lexer->input + lexer->pointer, "//", 2) != 0)
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    while(1) {
        ++lexer->pointer;
        char symbol = lexer->input[lexer->pointer];

        if(symbol == '\n')
            break;

        if(symbol == '\0')
            return (rac0a_lex_result_t) { RAC0A_ERROR };
    }

    ++lexer->pointer;

    token->lexeme = NULL; 
    token->type = RAC0A_TOKEN_COMMENT;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_comma(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if(lexer->input[lexer->pointer] != ',')
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->type = RAC0A_TOKEN_COMMA;
    token->lexeme = rac0a_string_copy(",");

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_character(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if (lexer->input[lexer->pointer] != '\'') 
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    int start = lexer->pointer;

    lexer->pointer += 2;
    if (lexer->input[lexer->pointer] != '\'') 
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    token->lexeme = rac0a_string_copy_len(&lexer->input[start], 3);
    token->type = RAC0A_TOKEN_NUMBER;

    lexer->pointer++;

    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_lex_result_t rac0a_lex_string(rac0a_token_t* token, rac0a_lexer_t* lexer) {
    if (lexer->input[lexer->pointer] != '"') 
        return (rac0a_lex_result_t) { RAC0A_ERROR };

    int start = lexer->pointer;

    while(1) {
        ++lexer->pointer;
        char symbol = lexer->input[lexer->pointer];

        if(symbol == '"')
            break;

        // Not sure about this one
        if(symbol == '\0')
            return (rac0a_lex_result_t) { RAC0A_ERROR };

        if(symbol == '\\') {
            ++lexer->pointer;
            symbol = lexer->input[lexer->pointer];

            if(symbol == '"') {
                continue;
            } else if(symbol == '\\') {
                continue;
            } else if(symbol == '/') {
                continue;
            } else if(symbol == 'b') {
                continue;
            } else if(symbol == 'f') {
                continue;
            } else if(symbol == 'n') {
                continue;
            } else if(symbol == 'r') {
                continue;
            } else if(symbol == 't') {
                continue;
            } else if(symbol == 'u') {
                ++lexer->pointer;

                for(int i = 0; i < 4; ++i) {
                    symbol = lexer->input[lexer->pointer];

                    if(!rac0a_in_range_include(symbol, '0', '9') && !rac0a_in_range_include(tolower(symbol), 'a', 'f'))
                        return (rac0a_lex_result_t) { RAC0A_ERROR };
                    
                    ++lexer->pointer;
                }

                --lexer->pointer;

                continue;
            } else {
                return (rac0a_lex_result_t) { RAC0A_ERROR };
            }
        }
    }

    ++lexer->pointer;

    // Save lexeme
    int length = lexer->pointer - start - 2;

    token->lexeme = malloc(length + 1);

    if(length != 0)
        strncpy(token->lexeme, &lexer->input[start + 1], length);

    token->lexeme[length] = '\0';
    token->type = RAC0A_TOKEN_STRING;
   
    return (rac0a_lex_result_t) { RAC0A_OK };
}

rac0a_token_t rac0a_next_token(rac0a_lexer_t* lexer) {
    rac0a_skip_whitespace(lexer);

    rac0a_token_t token;
    token.type = RAC0A_TOKEN_ERROR;
    token.lexeme = NULL;

    if(rac0a_lex_at(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_dollar(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_lparen(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_rparen(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_colon(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_label(&token, lexer).code == RAC0A_OK)
        return token;
    
    if(rac0a_lex_number(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_lbracket(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_rbracket(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_percent(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_string(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_ampersand(&token, lexer).code == RAC0A_OK)
        return token;

    if(rac0a_lex_comment(&token, lexer).code == RAC0A_OK)
        return token;
    
    if(rac0a_lex_comma(&token, lexer).code == RAC0A_OK)
        return token;
    
    if(rac0a_lex_eof(&token, lexer).code == RAC0A_OK)
        return token;

    return token;
}
