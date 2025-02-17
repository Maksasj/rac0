#include "rac0a_assembler.h"

void rac0a_assembler_program_push_byte(rac0a_assembler_t* assembler, rac0_byte_t byte) {
    byte_vector_push(&assembler->program, byte);
}

void rac0a_assembler_program_push_word(rac0a_assembler_t* assembler, rac0_value_t value) {
    rac0_byte_t* split = (rac0_byte_t*) &value;
    
    // value
    rac0a_assembler_program_push_byte(assembler, split[0]);
    rac0a_assembler_program_push_byte(assembler, split[1]);
    rac0a_assembler_program_push_byte(assembler, split[2]);
    rac0a_assembler_program_push_byte(assembler, split[3]);
    rac0a_assembler_program_push_byte(assembler, split[4]);
    rac0a_assembler_program_push_byte(assembler, split[5]);
    rac0a_assembler_program_push_byte(assembler, split[6]);
    rac0a_assembler_program_push_byte(assembler, split[7]);
}

void rac0a_assembler_program_push_instruction(rac0a_assembler_t* assembler, rac0_inst_t inst) {
    rac0_byte_t* split = (rac0_byte_t*) &inst.opcode;

    // opcode
    rac0a_assembler_program_push_byte(assembler, split[0]);
    rac0a_assembler_program_push_byte(assembler, split[1]);
    
    // value
    rac0a_assembler_program_push_word(assembler, inst.value);
}

rac0_value_t rac0a_assembler_program_get_pc(rac0a_assembler_t* assembler) {
    return assembler->program.size;
}

rac0a_hl_statement_list_t rac0a_assemble_run_1_pass(rac0a_hl_statement_list_t* input, vector_t* constvalues, vector_t* labels) {
    rac0a_hl_statement_list_t result;
    create_vector(&result, 100);
    
    rac0_u64_t pc = 0;

    PLUM_LOG(PLUM_INFO, "Running first assembler pass");
    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        PLUM_LOG(PLUM_DEBUG, "%d hl: [ %s ]", i, RAC0A_HL_STRING[statement->type]);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
            // todo free statement
            rac0a_constval_hl_info_t* info = (rac0a_constval_hl_info_t*) malloc(sizeof(rac0a_constval_hl_info_t));
            info->value = statement->as.constval.value;
            info->label = rac0a_string_copy(statement->as.constval.label);
            vector_push(constvalues, info);
        } else  if(statement->type == RAC0A_HL_TYPE_LABEL) {
            // todo free statement
            rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));
            info->pointer = pc;
            info->label = rac0a_string_copy(statement->as.label.label);
            vector_push(labels, info);
        } else  if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            vector_push(&result, statement);
            pc += sizeof(rac0_inst_t);
        } else  if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            vector_push(&result, statement);
            pc += sizeof(rac0_value_t);
        } else  if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            vector_push(&result, statement);
            pc += statement->as.byte_def.size; 
        } else  {
            PLUM_LOG(PLUM_WARNING, "Unreachable");
        }
    }

    return result;
}

void rac0a_assemble_run_2_pass(rac0a_assembler_t* assembler, rac0a_hl_statement_list_t* input, vector_t* constvalues, vector_t* labels) {
    PLUM_LOG(PLUM_INFO, "Running second assembler pass");
    
    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        PLUM_LOG(PLUM_DEBUG, "%d hl: [ %s ]", i, RAC0A_HL_STRING[statement->type]);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
            PLUM_LOG(PLUM_WARNING, "Unreachable");
        } else  if(statement->type == RAC0A_HL_TYPE_LABEL) {
            PLUM_LOG(PLUM_WARNING, "Unreachable");
        } else  if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            rac0a_hl_instruction_statement_t hl_instruction = statement->as.instruction;

            rac0_inst_t instruction;
            instruction.value = 0;

            if(hl_instruction.inst.type == RAC0A_HL_INSTRUCTION_TYPE_OPCODE) {
                instruction.opcode = hl_instruction.inst.as.opcode;
            } else if(hl_instruction.inst.type == RAC0A_HL_INSTRUCTION_TYPE_CONSTLABEL) {
                PLUM_LOG(PLUM_WARNING, "Not implemented");
            } else {
                PLUM_LOG(PLUM_WARNING, "Unreachable");
            }

            if(hl_instruction.value.type == RAC0A_HL_VALUE_TYPE_NONE) {
                // do nothing
            } else if(hl_instruction.value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
                int found = 0;

                for(int i = 0; i < vector_size(constvalues); ++i) {
                    rac0a_constval_hl_info_t* label_info = vector_get(constvalues, i);
                    
                    if(strcmp(hl_instruction.value.as.label, label_info->label) == 0) {
                        instruction.value = label_info->value;
                        ++found;
                        break;
                    }
                } 

                if(!found)
                    PLUM_LOG(PLUM_ERROR, "Constval with '%s' name is not defined", hl_instruction.value.as.label);
            } else if(hl_instruction.value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
                instruction.value = hl_instruction.value.as.value;
            } else if(hl_instruction.value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
                int found = 0;

                for(int i = 0; i < vector_size(labels); ++i) {
                    rac0a_label_hl_info_t* label_info = vector_get(labels, i);
                    
                    if(strcmp(hl_instruction.value.as.label, label_info->label) == 0) {
                        instruction.value = label_info->pointer;
                        ++found;
                        break;
                    }
                } 

                if(!found)
                    PLUM_LOG(PLUM_ERROR, "Label with '%s' name is not defined", hl_instruction.value.as.label);
            } else {
                PLUM_LOG(PLUM_WARNING, "Unreachable");
            }

            rac0a_assembler_program_push_instruction(assembler, instruction);
        } else  if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));
            info->pointer = rac0a_assembler_program_get_pc(assembler);
            info->label = rac0a_string_copy(statement->as.word_def.label);
            vector_push(labels, info);

            rac0a_assembler_program_push_word(assembler, statement->as.word_def.value);
        } else  if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));
            info->pointer = rac0a_assembler_program_get_pc(assembler);
            info->label = rac0a_string_copy(statement->as.byte_def.label);
            vector_push(labels, info);
            
            for(int i = 0; i < statement->as.byte_def.size; ++i)
                rac0a_assembler_program_push_byte(assembler, statement->as.byte_def.array[i]);
        } else  {
            PLUM_LOG(PLUM_WARNING, "Unreachable");
        }
    }
}

byte_vector_t rac0a_assemble_program(rac0a_hl_statement_list_t* hl_statements) {
    rac0a_assembler_t assembler;
    create_byte_vector(&assembler.program, 1000);

    // first pass we collect all labels and constvalues
    vector_t labels;
    create_vector(&labels, 100);

    vector_t constvalues;
    create_vector(&constvalues, 100);

    rac0a_hl_statement_list_t pass1 = rac0a_assemble_run_1_pass(hl_statements, &constvalues, &labels);
    rac0a_log_hl_statements("a.pass1.txt", &pass1);

    rac0a_assemble_run_2_pass(&assembler, &pass1, &constvalues, &labels);

    for(int i = 0; i < vector_size(&labels); ++i) {
        rac0a_label_hl_info_t* info = vector_get(&labels, i);
        PLUM_LOG(PLUM_EXPERIMENTAL, "Deteced label '%s' at 0x%.16llx", info->label, info->pointer);
    }

    return assembler.program;
}
