#include "rac0a_assembler.h"

void rac0a_assembler_program_push_instruction(byte_vector_t* vector, rac0_inst_t inst) {
    byte_vector_push16(vector, inst.opcode);
    byte_vector_push64(vector, inst.value);
}

rac0_value_t rac0a_assembler_program_get_pc(rac0a_assembler_t* assembler) {
    return assembler->program.size;
}

rac0a_hl_statement_list_t rac0a_assemble_run_1_pass(rac0a_hl_statement_list_t* input) {
    vector_t constvalues;
    create_vector(&constvalues, 100);

    rac0a_hl_statement_list_t result;
    create_vector(&result, 100);

    // first we collect all const values    
    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
            rac0a_constval_hl_info_t* info = (rac0a_constval_hl_info_t*) malloc(sizeof(rac0a_constval_hl_info_t));
            info->value = statement->as.constval.value;
            info->label = rac0a_string_copy(statement->as.constval.label);
            vector_push(&constvalues, info);

            rac0a_free_hl_statement(statement);
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
            PLUM_LOG(PLUM_WARNING, "Not implemented in rac0a_assemble_run_1_pass for RAC0A_HL_TYPE_CONSTBLOCK_DECL");
            rac0a_free_hl_statement(statement);
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
            vector_push(&result, statement);
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            vector_push(&result, statement);
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            vector_push(&result, statement);
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            vector_push(&result, statement);
        } else {
            PLUM_LOG(PLUM_ERROR, "Unreachable in rac0a_assemble_run_1_pass: hl statement type is not implemented");
        }
    }

    // then we replace all const value references with actuall value
    for(int i = 0; i < vector_size(&result); ++i) {
        rac0a_hl_statement_t* statement = vector_get(&result, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {

        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {

        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            rac0a_hl_instruction_statement_t* hl_instruction = &statement->as.instruction;

            if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_NONE) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
                int found = 0;

                for(int i = 0; i < vector_size(&constvalues); ++i) {
                    rac0a_constval_hl_info_t* label_info = vector_get(&constvalues, i);
                    
                    if(strcmp(hl_instruction->value.as.constval_label, label_info->label) == 0) {
                        hl_instruction->value.as.value = label_info->value;
                        hl_instruction->value.type = RAC0A_HL_VALUE_TYPE_NUMBER;
                        ++found;
                        break;
                    }
                } 

                if(!found)
                    PLUM_LOG(PLUM_ERROR, "Constval with '%s' name is not defined", hl_instruction->value.as.constval_label);
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
            } else {
                PLUM_LOG(PLUM_ERROR, "Unreachable");
            }
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        } else {
            PLUM_LOG(PLUM_ERROR, "Unreachable in rac0a_assemble_run_1_pass: hl statement type is not implemented");
        }
    }

    return result;
}

rac0a_hl_statement_list_t rac0a_assemble_run_2_pass(rac0a_hl_statement_list_t* input) {
    vector_t labels;
    create_vector(&labels, 100);

    rac0a_hl_statement_list_t result;
    create_vector(&result, 100);
    
    rac0_u64_t pc = 0;

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        PLUM_LOG(PLUM_DEBUG, "%d hl: [ %s ]", i, RAC0A_HL_STRING[statement->type]);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {

        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {

        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
            rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));
            info->pointer = pc;
            info->label = rac0a_string_copy(statement->as.label.label);
            vector_push(&labels, info);

            rac0a_free_hl_statement(statement);
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            vector_push(&result, statement);

            pc += sizeof(rac0_inst_t);
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));
            info->pointer = pc;
            info->label = rac0a_string_copy(statement->as.word_def.label);
            vector_push(&labels, info);
            vector_push(&result, statement);

            pc += sizeof(rac0_value_t);
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));
            info->pointer = pc;
            info->label = rac0a_string_copy(statement->as.byte_def.label);
            vector_push(&labels, info);
            vector_push(&result, statement);

            pc += statement->as.byte_def.size; 
        } else {
            PLUM_LOG(PLUM_ERROR, "Unreachable in rac0a_assemble_run_2_pass: hl statement type is not implemented");
        }
    }

    for(int i = 0; i < vector_size(&result); ++i) {
        rac0a_hl_statement_t* statement = vector_get(&result, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {

        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {

        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            rac0a_hl_instruction_statement_t* hl_instruction = &statement->as.instruction;

            if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_NONE) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
                int found = 0;

                for(int i = 0; i < vector_size(&labels); ++i) {
                    rac0a_label_hl_info_t* label_info = vector_get(&labels, i);
                    
                    if(strcmp(hl_instruction->value.as.label, label_info->label) == 0) {
                        hl_instruction->value.as.value = label_info->pointer;
                        hl_instruction->value.type = RAC0A_HL_VALUE_TYPE_NUMBER;
                        ++found;
                        break;
                    }
                } 

                if(!found)
                    PLUM_LOG(PLUM_ERROR, "Label with '%s' name is not defined", hl_instruction->value.as.label);
            } else {
                PLUM_LOG(PLUM_ERROR, "Unreachable");
            }
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {

        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {

        } else {
            PLUM_LOG(PLUM_ERROR, "Unreachable in rac0a_assemble_run_2_pass: hl statement type is not implemented");
        }
    }

    return result;
}


void rac0a_assemble_run_3_pass(rac0a_assembler_t* assembler, rac0a_hl_statement_list_t* input) {
    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        PLUM_LOG(PLUM_DEBUG, "%d hl: [ %s ]", i, RAC0A_HL_STRING[statement->type]);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {

        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {

        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            rac0a_hl_instruction_statement_t hl_instruction = statement->as.instruction;

            rac0_inst_t instruction;
            instruction.value = 0;

            if(hl_instruction.inst.type == RAC0A_HL_INSTRUCTION_TYPE_OPCODE) {
                instruction.opcode = hl_instruction.inst.as.opcode;
            } else if(hl_instruction.inst.type == RAC0A_HL_INSTRUCTION_TYPE_CONSTLABEL) {
                PLUM_LOG(PLUM_WARNING, "Not implemented");
            } else {
                PLUM_LOG(PLUM_ERROR, "Unreachable");
            }

            if(hl_instruction.value.type == RAC0A_HL_VALUE_TYPE_NONE) {
                // do nothing
            } else if(hl_instruction.value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
                // do nothing
            } else if(hl_instruction.value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
                instruction.value = hl_instruction.value.as.value;
            } else if(hl_instruction.value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
                // do nothing
            } else {
                PLUM_LOG(PLUM_ERROR, "Unreachable");
            }

            rac0a_assembler_program_push_instruction(&assembler->program, instruction);
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            byte_vector_push64(&assembler->program, statement->as.word_def.value);
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            for(int i = 0; i < statement->as.byte_def.size; ++i)
                byte_vector_push_byte(&assembler->program, statement->as.byte_def.array[i]);
        } else {
            PLUM_LOG(PLUM_ERROR, "Unreachable in rac0a_assemble_run_3_pass: hl statement type is not implemented");
        }
    }
}

byte_vector_t rac0a_assemble_program(rac0a_hl_statement_list_t* hl_statements) {
    rac0a_assembler_t assembler;
    create_byte_vector(&assembler.program, 1000);

    // first pass we collect all labels and constvalues
    rac0a_hl_statement_list_t pass1 = rac0a_assemble_run_1_pass(hl_statements);
    rac0a_log_hl_statements("a.pass1.txt", &pass1);

    // then we calculate actual addresses and replace all label references
    rac0a_hl_statement_list_t pass2 = rac0a_assemble_run_2_pass(&pass1);
    rac0a_log_hl_statements("a.pass2.txt", &pass2);

    rac0a_assemble_run_3_pass(&assembler, &pass2);

    return assembler.program;
}
