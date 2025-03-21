#include "rac0a_hl.h"

void rac0a_free_hl_constval_statement(rac0a_hl_constval_statement_t statement) {
    // nothing
}

void rac0a_free_hl_constblock_statement(rac0a_hl_constblock_statement_t statement) {
    free(statement.label);
    rac0a_free_hl_statement_list(&statement.statements);
}

void rac0a_free_hl_label_statement(rac0a_hl_label_statement_t statement) {
    free(statement.label);
}

void rac0a_free_hl_instruction_statement(rac0a_hl_instruction_statement_t statement) {
    if(statement.inst.type == RAC0A_HL_INSTRUCTION_TYPE_OPCODE) {
        // nothing
    } else if(statement.inst.type == RAC0A_HL_INSTRUCTION_TYPE_CONSTLABEL) {
        free(statement.inst.as.constlabel);
    } else {
        PLUM_UNREACHABLE();
    }

    if(statement.value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
        // nothing
    } else if(statement.value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
        free(statement.value.as.label);
    } else if(statement.value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
        free(statement.value.as.constval_label);
    } else if(statement.value.type == RAC0A_HL_VALUE_TYPE_NONE) {
        // nothing
    } else {
        PLUM_UNREACHABLE();
    }
}

void rac0a_free_hl_word_def_statement(rac0a_hl_word_def_statement_t statement) {
    if(statement.value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
        // nothing
    } else if(statement.value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
        free(statement.value.as.label);
    } else if(statement.value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
        free(statement.value.as.constval_label);
    } else if(statement.value.type == RAC0A_HL_VALUE_TYPE_NONE) {
        // nothing
    } else {
        PLUM_UNREACHABLE();
    }

    free(statement.label);
}

void rac0a_free_hl_byte_def_statement(rac0a_hl_byte_def_statement_t statement) {
    free(statement.label);
    free(statement.array);
}

void rac0a_free_hl_statement(rac0a_hl_statement_t* statement) {
    if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        rac0a_free_hl_constval_statement(statement->as.constval);
    } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        rac0a_free_hl_constblock_statement(statement->as.constblock);
    } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
        rac0a_free_hl_label_statement(statement->as.label);
    } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
        rac0a_free_hl_instruction_statement(statement->as.instruction);
    } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
        rac0a_free_hl_word_def_statement(statement->as.word_def);
    } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        rac0a_free_hl_byte_def_statement(statement->as.byte_def);
    } else {
        PLUM_UNREACHABLE();
    }

    free(statement);
}

void rac0a_free_hl_statement_list(rac0a_hl_statement_list_t* list) {
    for(int i = 0; i < vector_size(list); ++i)
        rac0a_free_hl_statement(vector_get(list, i));

    free_vector(list);
}

void rac0a_log_hl_statements(string_t file_path, rac0a_hl_statement_list_t* list) {
    FILE *out_file = fopen(file_path, "w");
    rac0a_log_hl_statements_file(out_file, list);
    fclose(out_file);
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
            fprintf(file, "[ %.6d ] [ INSTR ] ", i);

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
            fprintf(file, "[ %.6d ] [ WODEF ] ", i);

            if(statement->as.word_def.value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
                fprintf(file, "0x%.16llx ", statement->as.word_def.value.as.value);
            } else if(statement->as.word_def.value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
                fprintf(file, "&%s", statement->as.word_def.value.as.label);
            } else if(statement->as.word_def.value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
                fprintf(file, "$%s", statement->as.word_def.value.as.constval_label);
            }

            fprintf(file, "\n");
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {            
            fprintf(file, "[ %.6d ] [ BYDEF ] \"%s\"\n", i, statement->as.byte_def.array);
        } else {
            PLUM_UNREACHABLE();
        }
    }
}