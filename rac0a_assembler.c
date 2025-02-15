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

byte_vector_t rac0a_assemble_program(vector_t* hl_statements) {
    int size = vector_size(hl_statements);
    
    rac0a_assembler_t assembler;
    create_vector(&assembler.labels, 100);
    create_byte_vector(&assembler.program, 1000);
    
    for(int i = 0; i < size; ++i) {
        rac0a_hl_statement_t* statement = vector_get(hl_statements, i);

        PLUM_LOG(PLUM_DEBUG, "%d hl: [ %s ]", i, RAC0A_HL_STRING[statement->type]);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL) {

        } else  if(statement->type == RAC0A_HL_TYPE_LABEL) {
            rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));
            info->pointer = rac0a_assembler_program_get_pc(&assembler);
            info->label = rac0a_string_copy(statement->as.label.label);
            vector_push(&assembler.labels, info);
        } else  if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            rac0a_assembler_program_push_instruction(&assembler, statement->as.instruction.inst);
        } else  if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));
            info->pointer = rac0a_assembler_program_get_pc(&assembler);
            info->label = rac0a_string_copy(statement->as.label.label);
            vector_push(&assembler.labels, info);

            rac0a_assembler_program_push_word(&assembler, statement->as.word_def.value);
        } else  if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {

        } else  {

        }
    }

    for(int i = 0; i < vector_size(&assembler.labels); ++i) {
        rac0a_label_hl_info_t* info = vector_get(&assembler.labels, i);
        PLUM_LOG(PLUM_EXPERIMENTAL, "Deteced label '%s' at 0x%.16llx", info->label, info->pointer);
    }

    return assembler.program;
}
