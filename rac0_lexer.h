#ifndef RAC0_LEXER_H
#define RAC0_LEXER_H

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Lexer
typedef enum {
    RAC0A_TOKEN_AT = 1,     
    RAC0A_TOKEN_DOLLAR = 2,     

    RAC0A_TOKEN_L_PAREN = 3,
    RAC0A_TOKEN_R_PAREN = 4,
    RAC0A_TOKEN_COLON = 5,

    RAC0A_TOKEN_LABEL = 6,
    RAC0A_TOKEN_NUMBER = 7,

    RAC0A_TOKEN_L_BRACKET = 8,
    RAC0A_TOKEN_R_BRACKET = 9,

    RAC0A_TOKEN_PERCENT = 10,
    RAC0A_TOKEN_STRING = 11,

    RAC0A_TOKEN_EOF = 0,     
    RAC0A_TOKEN_ERROR = -1
} rac0a_token_type_t;

typedef struct {
    rac0a_token_type_t type;
    char *lexeme;
} rac0a_token_t;

typedef struct {
    const char* input;
    int pointer;
} rac0a_lexer_t;

typedef enum {
    RAC0A_OK,
    RAC0A_ERROR
} rac0a_result_code_t;

typedef struct {
    rac0a_result_code_t code;
} rac0a_lex_result_t;

typedef unsigned char rac0a_boolean_t;

char* rac0a_string_copy(const char* input) {
    unsigned long long len = strlen(input) + 1;
    char* out =  malloc(sizeof(char) * len);
    memcpy(out, input, len);
    return out;
}

char* rac0a_string_copy_len(const char* input, unsigned long long size) {
    unsigned long long len = size + 1;
    char* out =  calloc(len, sizeof(char));
    memcpy(out, input, len);
    return out;
}

// lexer
void rac0a_free_token(rac0a_token_t token) {
    if(token.lexeme != NULL)
        free(token.lexeme);
}

void rac0a_skip_whitespace(rac0a_lexer_t* lexer);
rac0a_token_t rac0a_next_token(rac0a_lexer_t* lexer);

// lexer
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

    while(1) {
        if(!isalpha(lexer->input[lexer->pointer]) && lexer->input[lexer->pointer] != '_')
            break;

        ++lexer->pointer;
    }

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

rac0a_boolean_t rac0a_in_range_include(char value, char min, char max) {
    return (value >= min) && (value <= max); 
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
    int length = lexer->pointer - start;

    token->lexeme = malloc(length + 1);
    strncpy(token->lexeme, &lexer->input[start], length);

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
        
    if(rac0a_lex_eof(&token, lexer).code == RAC0A_OK)
        return token;

    return token;
}

#endif
