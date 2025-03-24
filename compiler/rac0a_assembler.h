#ifndef RAC0_ASSEMBLER_H
#define RAC0_ASSEMBLER_H

#include "rac0a_parser.h"

typedef struct {
    char* input_filename;
    char* output_filename;
    rac0_value_t dump;
} rac0a_compiler_configuration_t;

typedef struct {
    rac0a_result_code_t code;

    union {
        struct {
            const char* message;
            rac0_u64_t pointer;
        } error;
        struct {

        } ok;
    } as;
} rac0a_assemble_result_t;

rac0a_assemble_result_t rac0a_assemble_result_ok();
rac0a_assemble_result_t rac0a_assemble_result_failed();
rac0a_assemble_result_t rac0a_assemble_result_error(const char* message, rac0_u64_t pointer);

typedef struct {
    char* label;
    rac0_value_t value;
} rac0a_constval_hl_info_t;

rac0a_constval_hl_info_t* rac0a_create_constval_hl_info(const string_t label, rac0_value_t value);
void rac0a_free_constval_hl_info(rac0a_constval_hl_info_t* info);

rac0a_constval_hl_info_t* rac0a_get_constval_hl_info(vector_t* container, const string_t label);

typedef struct {
    char* label;
    rac0_value_t pointer;
} rac0a_label_hl_info_t;

rac0a_label_hl_info_t* rac0a_create_label_hl_info(const string_t label, rac0_value_t pointer);
void rac0a_free_label_hl_info(rac0a_label_hl_info_t* info);

rac0a_label_hl_info_t* rac0a_get_label_hl_info(vector_t* container, const string_t label);

void rac0a_byte_vector_push_instruction(byte_vector_t* vector, rac0_inst_t inst);

rac0a_assemble_result_t rac0a_assemble_run_validate_label_multiple_pass(rac0a_hl_statement_list_t* input);
rac0a_assemble_result_t rac0a_assemble_run_validate_constval_multiple_pass(rac0a_hl_statement_list_t* input);
rac0a_assemble_result_t rac0a_assemble_run_validate_constblock_multiple_pass(rac0a_hl_statement_list_t* input);
rac0a_assemble_result_t rac0a_assemble_run_validate_byte_def_multiple_pass(rac0a_hl_statement_list_t* input);
rac0a_assemble_result_t rac0a_assemble_run_validate_word_def_multiple_pass(rac0a_hl_statement_list_t* input);
rac0a_assemble_result_t rac0a_assemble_run_validate_const_multiple_pass(rac0a_hl_statement_list_t* input);
rac0a_assemble_result_t rac0a_assemble_run_validate_all_multiple_pass(rac0a_hl_statement_list_t* input);

rac0a_assemble_result_t rac0a_assemble_run_1_pass(rac0a_hl_statement_list_t* result, rac0a_hl_statement_list_t* input);
rac0a_assemble_result_t rac0a_assemble_run_2_pass(rac0a_hl_statement_list_t* result, rac0a_hl_statement_list_t* input);
byte_vector_t rac0a_assemble_run_final_pass(rac0a_hl_statement_list_t* input);

rac0a_assemble_result_t rac0a_assemble_program(rac0a_compiler_configuration_t* configuration, byte_vector_t* vector, rac0a_hl_statement_list_t* hl_statements);

#endif
