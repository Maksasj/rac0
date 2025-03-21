#ifndef RAC0A_LEXER_H
#define RAC0A_LEXER_H

#include <ctype.h>

#include "rac0_base.h"
#include "rac0_utils.h"

typedef enum {
    RAC0A_OK,
    RAC0A_FAILED,
    RAC0A_ERROR,
} rac0a_result_code_t;

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
    RAC0A_TOKEN_AMPERSAND = 12,

    RAC0A_TOKEN_COMMENT = 13,  
    RAC0A_TOKEN_COMMA = 14,   
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

typedef struct {
    rac0a_result_code_t code;
} rac0a_lex_result_t;

// lexer
void rac0a_free_token(rac0a_token_t token);
void rac0a_skip_whitespace(rac0a_lexer_t* lexer);

// tokens
rac0a_lex_result_t rac0a_lex_at(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_dollar(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_lparen(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_rparen(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_colon(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_label(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_hex_number(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_binary_number(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_number(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_eof(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_lbracket(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_rbracket(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_percent(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_ampersand(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_comment(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_comma(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_lex_result_t rac0a_lex_string(rac0a_token_t* token, rac0a_lexer_t* lexer);

rac0a_token_t rac0a_next_token(rac0a_lexer_t* lexer);

#endif
