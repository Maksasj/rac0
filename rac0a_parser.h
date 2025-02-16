#ifndef RAC0A_PARSER_H
#define RAC0A_PARSER_H

#include "haul/haul.h"

#include "rac0a_lexer.h"
#include "rac0_utils.h"

typedef enum {
    RAC0A_HL_TYPE_CONSTVAL_DECL = 0,
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
    char* label;
    rac0_value_t value;
} rac0a_hl_constval_statement_t;

typedef struct {
    char* label;
} rac0a_hl_label_statement_t;

typedef enum {
    RAC0A_HL_VALUE_TYPE_NUMBER,
    RAC0A_HL_VALUE_TYPE_LABEL_POINTER,
    RAC0A_HL_VALUE_TYPE_CONSTVAL,

    RAC0A_HL_VALUE_TYPE_NONE,
} rac0a_hl_value_type_t;

typedef struct {
    rac0a_hl_value_type_t type;

    union {
        rac0_value_t value;
        char* label;
        char* constval_label;
    } as;
} rac0a_hl_value_t;

typedef enum {
    RAC0A_HL_INSTRUCTION_TYPE_OPCODE,
    RAC0A_HL_INSTRUCTION_TYPE_CONSTLABEL,
} rac0a_hl_instruction_type_t;

typedef struct {
    rac0a_hl_instruction_type_t type;

    union {
        rac0_opcode_t opcode;
        char* constlabel;
    } as;
} rac0a_hl_instruction_t;

typedef struct {
    rac0a_hl_instruction_t inst;
    rac0a_hl_value_t value;
} rac0a_hl_instruction_statement_t;

typedef struct {
    char* label;
    rac0_value_t value;
} rac0a_hl_word_def_statement_t;

typedef struct {
    char* label;
    
    rac0_byte_t* array;
    rac0_value_t size;
} rac0a_hl_byte_def_statement_t;

typedef struct {
    rac0a_hl_statement_type_t type;

    union {
        rac0a_hl_constval_statement_t constval;
        rac0a_hl_label_statement_t label;
        rac0a_hl_instruction_statement_t instruction;
        rac0a_hl_word_def_statement_t word_def;
        rac0a_hl_byte_def_statement_t byte_def;
    } as;
} rac0a_hl_statement_t;

typedef struct {
    rac0a_result_code_t code;
} rac0a_parse_result_t;

typedef struct {
    rac0a_lexer_t lexer;

    vector_t hl_statements;
} rac0a_parser_t;

rac0a_parse_result_t rac0a_parse_token(rac0a_parser_t* parser, rac0a_token_type_t type, rac0a_token_t* ret);

rac0a_parse_result_t rac0a_parse_exact_word(rac0a_parser_t* parser, const char* lexem);

rac0a_parse_result_t rac0a_parse_include_statement(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_constval_definition(rac0a_parser_t* parser, rac0a_hl_constval_statement_t* constval);

rac0a_parse_result_t rac0a_parse_label_definition(rac0a_parser_t* parser, rac0a_hl_label_statement_t* label);

rac0a_parse_result_t rac0a_parse_eof(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_number(rac0a_parser_t* parser, rac0_value_t* value);

rac0a_parse_result_t rac0a_parse_const_thing_usage(rac0a_parser_t* parser, char** label);

rac0a_parse_result_t rac0a_parse_label_pointer(rac0a_parser_t* parser, char** label);

rac0a_parse_result_t rac0a_parse_value(rac0a_parser_t* parser, rac0a_hl_value_t* value);

rac0a_parse_result_t rac0a_parse_instruction_noarg(rac0a_parser_t* parser, const char* lexem);

rac0a_parse_result_t rac0a_parse_instruction_arg(rac0a_parser_t* parser, const char* lexem, rac0a_hl_value_t* value);

rac0a_parse_result_t rac0a_parse_instruction(rac0a_parser_t* parser, rac0a_hl_instruction_statement_t* inst);

rac0a_parse_result_t rac0a_parse_byte_definition(rac0a_parser_t* parser, rac0a_hl_byte_def_statement_t* value);

rac0a_parse_result_t rac0a_parse_constblock_usage(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_word_definition(rac0a_parser_t* parser, rac0a_hl_word_def_statement_t* value);

rac0a_parse_result_t rac0a_parse_statement_list(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_module_definition(rac0a_parser_t* parser);

vector_t rac0a_parse_program(const char* input);

#endif
