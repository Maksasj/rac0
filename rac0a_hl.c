#include "rac0a_hl.h"

void rac0a_log_hl_statements(string_t file_path, rac0a_hl_statement_list_t* list) {
    FILE *out_file = fopen(file_path, "w");
    rac0a_log_hl_statements_file(out_file, list);

}

void rac0a_log_hl_statements_file(FILE* file, rac0a_hl_statement_list_t* list) {
    for(int i = 0; i < vector_size(list); ++i) {
        rac0a_hl_statement_t* statement = vector_get(list, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
            fprintf(file, "[ %.6d ] [ COVAL ] %s 0x%.16llx\n", i,  statement->as.constval.label, statement->as.constval.value);
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
            fprintf(file, "[ %.6d ] [ COBLO ]\n", i);
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
            fprintf(file, "[ %.6d ] [ LABEL ] %s\n", i, statement->as.label.label);
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            fprintf(file, "[ %.6d ] [ INSTR ] ", i);\

            if(statement->as.instruction.inst.type == RAC0A_HL_INSTRUCTION_TYPE_OPCODE) {
                fprintf(file, "%s ", RAC0_OPCODE_STRING[statement->as.instruction.inst.as.opcode]);
            } else if(statement->as.instruction.inst.type == RAC0A_HL_INSTRUCTION_TYPE_CONSTLABEL) {
                fprintf(file, "$%s ", statement->as.instruction.inst.as.constlabel);
            }

            if(statement->as.instruction.value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
                fprintf(file, "0x%.16llx ", statement->as.instruction.value.as.value);
            } else if(statement->as.instruction.value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
                fprintf(file, "&%s", statement->as.instruction.value.as.label);
            } else if(statement->as.instruction.value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
                fprintf(file, "$%s", statement->as.instruction.value.as.constval_label);
            }

            fprintf(file, "\n");
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            fprintf(file, "[ %.6d ] [ WODEF ] 0x%.16llx\n", i, statement->as.word_def.value);
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {            
            fprintf(file, "[ %.6d ] [ BYDEF ] \"%s\"\n", i, statement->as.byte_def.array);
        } else {
            PLUM_LOG(PLUM_ERROR, "Unreachable");
        }
    }
}