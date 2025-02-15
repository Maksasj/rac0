#ifndef RAC0A_PARSER_H
#define RAC0A_PARSER_H

#include "haul/haul.h"

#include "rac0a_lexer.h"
#include "rac0a_utils.h"

typedef enum {
    RAC0A_HL_TYPE_CONSTVAL = 0,
    RAC0A_HL_TYPE_LABEL = 1,
    RAC0A_HL_TYPE_INSTRUCTION = 2,
    RAC0A_HL_TYPE_WORD_DEF = 3,
    RAC0A_HL_TYPE_BYTE_DEF = 4,
} rac0a_hl_statement_type_t;

static char* RAC0A_HL_STRING[] = {
    "CONSTVAL",
    "LABEL",
    "INSTRUCTION",
    "WORD_DEF",
    "BYTE_DEF"
};

typedef struct {
    rac0a_hl_statement_type_t type;

    union {
        int constval;
        int label;
        int instruction;
        int word_def;
        int byte_def;
    } as;
} rac0a_hl_statement_t;

typedef struct {
    vector_t hl_statements;
} rac0a_program_t;

typedef struct {
    rac0a_result_code_t code;
} rac0a_parse_result_t;

rac0a_parse_result_t rac0a_parse_token(rac0a_lexer_t* lexer, rac0a_token_type_t type);

rac0a_parse_result_t rac0a_parse_exact_word(rac0a_lexer_t* lexer, const char* lexem);

rac0a_parse_result_t rac0a_parse_include_statement(rac0a_program_t* program, rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_constval_definition(rac0a_program_t* program, rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_label_definition(rac0a_program_t* program, rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_eof(rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_number(rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_label_usage(rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_instruction_noarg(rac0a_lexer_t* lexer, const char* lexem);

rac0a_parse_result_t rac0a_parse_instruction_arg(rac0a_lexer_t* lexer, const char* lexem);

rac0a_parse_result_t rac0a_parse_instruction(rac0a_program_t* program, rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_byte_definition(rac0a_program_t* program, rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_word_definition(rac0a_program_t* program, rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_statement_list(rac0a_program_t* program, rac0a_lexer_t* lexer);

rac0a_parse_result_t rac0a_parse_module(rac0a_program_t* program, rac0a_lexer_t* lexer);

void rac0a_parse_program(rac0a_program_t* program, const char* input);

#endif
