#ifndef RAC0_ASSEMBLER_H
#define RAC0_ASSEMBLER_H

#include "rac0a_parser.h"

typedef struct {
    char* label;
    rac0_value_t value;
} rac0a_constval_hl_info_t;

typedef struct {
    char* label;
    rac0_value_t pointer;
} rac0a_label_hl_info_t;

typedef struct {
    byte_vector_t program;
} rac0a_assembler_t;

void rac0a_assembler_program_push_instruction(byte_vector_t* vector, rac0_inst_t inst);

rac0_value_t rac0a_assembler_program_get_pc(rac0a_assembler_t* assembler);

rac0a_hl_statement_list_t rac0a_assemble_run_1_pass(rac0a_hl_statement_list_t* input);
rac0a_hl_statement_list_t rac0a_assemble_run_2_pass(rac0a_hl_statement_list_t* input);
void rac0a_assemble_run_3_pass(rac0a_assembler_t* assembler, rac0a_hl_statement_list_t* input);

byte_vector_t rac0a_assemble_program(vector_t* hl_statements);

#endif
