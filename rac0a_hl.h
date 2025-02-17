#ifndef RAC0A_HL_H
#define RAC0A_HL_H

#include <stdio.h>

#include "haul/haul.h"
#include "rac0_utils.h"

typedef enum {
    RAC0A_HL_TYPE_CONSTVAL_DECL = 0,
    RAC0A_HL_TYPE_CONSTBLOCK_DECL = 1,
    RAC0A_HL_TYPE_LABEL = 2,
    RAC0A_HL_TYPE_INSTRUCTION = 3,
    RAC0A_HL_TYPE_WORD_DEF = 4,
    RAC0A_HL_TYPE_BYTE_DEF = 5,
} rac0a_hl_statement_type_t;

static char* RAC0A_HL_STRING[] = {
    "CONSTVAL",
    "CONSTBLOCK",
    "LABEL",
    "INSTRUCTION",
    "WORD_DEF",
    "BYTE_DEF"
};

typedef struct {
    string_t label;
    rac0_value_t value;
} rac0a_hl_constval_statement_t;

typedef struct {
    char* label;
    vector_t statements;
} rac0a_hl_constblock_statement_t;

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
        rac0a_hl_constblock_statement_t constblock;
        rac0a_hl_label_statement_t label;
        rac0a_hl_instruction_statement_t instruction;
        rac0a_hl_word_def_statement_t word_def;
        rac0a_hl_byte_def_statement_t byte_def;
    } as;
} rac0a_hl_statement_t;

typedef vector_t rac0a_hl_statement_list_t;

void rac0a_log_hl_statements(string_t file_path, rac0a_hl_statement_list_t* list);
void rac0a_log_hl_statements_file(FILE* file, rac0a_hl_statement_list_t* list);

#endif