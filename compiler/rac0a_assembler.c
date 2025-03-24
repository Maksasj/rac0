#include "rac0a_assembler.h"

rac0a_assemble_result_t rac0a_assemble_result_ok() {
    return (rac0a_assemble_result_t) { RAC0A_OK };
}

rac0a_assemble_result_t rac0a_assemble_result_failed(const char* message, rac0_u64_t pointer) {
    return (rac0a_assemble_result_t) { 
        .code = RAC0A_FAILED, 
        .as.error = {
            .message = message,
            .pointer = pointer,
        }
    };
}

rac0a_assemble_result_t rac0a_assemble_result_error(const char* message, rac0_u64_t pointer) {
    return (rac0a_assemble_result_t) { 
        .code = RAC0A_ERROR, 
        .as.error = {
            .message = message,
            .pointer = pointer,
        }
    };
}

rac0a_constval_hl_info_t* rac0a_create_constval_hl_info(const string_t label, rac0_value_t value) {
    rac0a_constval_hl_info_t* info = (rac0a_constval_hl_info_t*) malloc(sizeof(rac0a_constval_hl_info_t));

    info->label = rac0a_string_copy(label);
    info->value = value;

    return info;
}

void rac0a_free_constval_hl_info(rac0a_constval_hl_info_t* info) {
    free(info->label);
    free(info);
}

rac0a_constval_hl_info_t* rac0a_get_constval_hl_info(vector_t* container, const string_t label) {
    for(int i = 0; i < vector_size(container); ++i) {
        rac0a_constval_hl_info_t* label_info = vector_get(container, i);
                    
        if(strcmp(label, label_info->label) != 0)
            continue;

        return label_info;
    }

    return NULL; 
}

rac0a_label_hl_info_t* rac0a_create_label_hl_info(const string_t label, rac0_value_t pointer) {
    rac0a_label_hl_info_t* info = (rac0a_label_hl_info_t*) malloc(sizeof(rac0a_label_hl_info_t));

    info->label = rac0a_string_copy(label);
    info->pointer = pointer;

    return info;
}

void rac0a_free_label_hl_info(rac0a_label_hl_info_t* info) {
    free(info->label);
    free(info);
}

rac0a_label_hl_info_t* rac0a_get_label_hl_info(vector_t* container, const string_t label) {
    for(int i = 0; i < vector_size(container); ++i) {
        rac0a_label_hl_info_t* label_info = vector_get(container, i);
                    
        if(strcmp(label, label_info->label) != 0)
            continue;

        return label_info;
    }

    return NULL; 
}

void rac0a_byte_vector_push_instruction(byte_vector_t* vector, rac0_inst_t inst) {
    byte_vector_push16(vector, inst.opcode);
    byte_vector_push64(vector, inst.value);
}

rac0a_assemble_result_t rac0a_assemble_run_validate_label_multiple_pass(rac0a_hl_statement_list_t* input) {
    string_set_t set;
    create_string_set(&set, 1000); 

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
            if(string_set_has(&set, statement->as.label.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.label.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        } else {
            PLUM_UNREACHABLE();
        }
    }

    free_string_set(&set);

    return rac0a_assemble_result_ok();
}

rac0a_assemble_result_t rac0a_assemble_run_validate_constval_multiple_pass(rac0a_hl_statement_list_t* input) {
    string_set_t set;
    create_string_set(&set, 1000); 

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
            if(string_set_has(&set, statement->as.constval.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Constval with a same label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.constval.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        } else {
            PLUM_UNREACHABLE();
        }
    }

    free_string_set(&set);

    return rac0a_assemble_result_ok();
}

rac0a_assemble_result_t rac0a_assemble_run_validate_byte_def_multiple_pass(rac0a_hl_statement_list_t* input) {
    string_set_t set;
    create_string_set(&set, 1000); 

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            if(string_set_has(&set, statement->as.byte_def.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Byte array with same label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.byte_def.label);
            }
        } else {
            PLUM_UNREACHABLE();
        }
    }

    free_string_set(&set);

    return rac0a_assemble_result_ok();
}

rac0a_assemble_result_t rac0a_assemble_run_validate_constblock_multiple_pass(rac0a_hl_statement_list_t* input) {
    string_set_t set;
    create_string_set(&set, 1000); 

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
            if(string_set_has(&set, statement->as.constblock.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Constblock with same label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.constblock.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        } else {
            PLUM_UNREACHABLE();
        }
    }

    free_string_set(&set);

    return rac0a_assemble_result_ok();
}

rac0a_assemble_result_t rac0a_assemble_run_validate_word_def_multiple_pass(rac0a_hl_statement_list_t* input) {
    string_set_t set;
    create_string_set(&set, 1000); 

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            if(string_set_has(&set, statement->as.word_def.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Word with same label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.word_def.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        } else {
            PLUM_UNREACHABLE();
        }
    }

    free_string_set(&set);

    return rac0a_assemble_result_ok();
}

rac0a_assemble_result_t rac0a_assemble_run_validate_const_multiple_pass(rac0a_hl_statement_list_t* input) {
    string_set_t set;
    create_string_set(&set, 1000); 

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
            if(string_set_has(&set, statement->as.constval.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Const statement a same label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.constval.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
            if(string_set_has(&set, statement->as.constblock.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Const statement with a same label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.constblock.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        } else {
            PLUM_UNREACHABLE();
        }
    }

    free_string_set(&set);

    return rac0a_assemble_result_ok();
}

rac0a_assemble_result_t rac0a_assemble_run_validate_all_multiple_pass(rac0a_hl_statement_list_t* input) {
    string_set_t set;
    create_string_set(&set, 1000); 

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
            if(string_set_has(&set, statement->as.constval.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Statement label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.constval.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
            if(string_set_has(&set, statement->as.constblock.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Statement label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.constblock.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
            if(string_set_has(&set, statement->as.label.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Statement label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.label.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            if(string_set_has(&set, statement->as.word_def.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Statement label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.label.label);
            }
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            if(string_set_has(&set, statement->as.byte_def.label)) {
                free_string_set(&set);
                return rac0a_assemble_result_failed("Statement label is already defined", statement->pointer);
            } else {
                string_set_insert(&set, statement->as.label.label);
            }
        } else {
            PLUM_UNREACHABLE();
        }
    }

    free_string_set(&set);

    return rac0a_assemble_result_ok();
}

rac0a_assemble_result_t rac0a_assemble_run_1_pass(rac0a_hl_statement_list_t* result, rac0a_hl_statement_list_t* input) {
    vector_t constvalues;
    create_vector(&constvalues, 100);

    // first we collect all const values    
    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
            vector_push(&constvalues, rac0a_create_constval_hl_info(statement->as.constval.label, statement->as.constval.value));
            rac0a_free_hl_statement(statement);
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
            PLUM_LOG(PLUM_WARNING, "Not implemented in rac0a_assemble_run_1_pass for RAC0A_HL_TYPE_CONSTBLOCK_DECL");
            rac0a_free_hl_statement(statement);
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
            vector_push(result, statement);
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            vector_push(result, statement);
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            vector_push(result, statement);
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            vector_push(result, statement);
        } else {
            PLUM_UNREACHABLE();
        }
    }

    // then we replace all const value references with actuall values
    for(int i = 0; i < vector_size(result); ++i) {
        rac0a_hl_statement_t* statement = vector_get(result, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            rac0a_hl_instruction_statement_t* hl_instruction = &statement->as.instruction;

            if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_NONE) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
                rac0a_constval_hl_info_t* info = rac0a_get_constval_hl_info(&constvalues, hl_instruction->value.as.constval_label);

                if(info == NULL) {
                    for(int i = 0; i < vector_size(&constvalues); ++i)
                        rac0a_free_constval_hl_info(vector_get(&constvalues, i));
            
                    free_vector(&constvalues);
                    return rac0a_assemble_result_failed("Constval is not defined", statement->pointer);
                } else {
                    hl_instruction->value.as.value = info->value;
                    hl_instruction->value.type = RAC0A_HL_VALUE_TYPE_NUMBER;
                }
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
            } else {
                PLUM_UNREACHABLE();
            }
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            rac0a_hl_word_def_statement_t* word_def = &statement->as.word_def;
            
            if(word_def->value.type == RAC0A_HL_VALUE_TYPE_NONE) {
                PLUM_UNREACHABLE();
            } else if(word_def->value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
                rac0a_constval_hl_info_t* info = rac0a_get_constval_hl_info(&constvalues, word_def->value.as.constval_label);

                if(info == NULL) {
                    for(int i = 0; i < vector_size(&constvalues); ++i)
                    rac0a_free_constval_hl_info(vector_get(&constvalues, i));
        
                    free_vector(&constvalues);
                    return rac0a_assemble_result_failed("Constval is not defined", statement->pointer);
                } else {
                    word_def->value.as.value = info->value;
                    word_def->value.type = RAC0A_HL_VALUE_TYPE_NUMBER;
                }
            } else if(word_def->value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
            } else if(word_def->value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
            } else {
                PLUM_UNREACHABLE();
            }
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        } else {
            PLUM_UNREACHABLE();
        }
    }

    for(int i = 0; i < vector_size(&constvalues); ++i)
        rac0a_free_constval_hl_info(vector_get(&constvalues, i));

    free_vector(&constvalues);
    
    return rac0a_assemble_result_ok();
}

rac0a_assemble_result_t rac0a_assemble_run_2_pass(rac0a_hl_statement_list_t* result, rac0a_hl_statement_list_t* input) {
    vector_t labels;
    create_vector(&labels, 100);
    
    rac0_u64_t pc = 0;

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
            vector_push(&labels, rac0a_create_label_hl_info(statement->as.label.label, pc));
            rac0a_free_hl_statement(statement);
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            vector_push(result, statement);
            pc += sizeof(rac0_inst_t);
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            vector_push(&labels, rac0a_create_label_hl_info(statement->as.label.label, pc));
            vector_push(result, statement);
            pc += sizeof(rac0_value_t);
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            vector_push(&labels, rac0a_create_label_hl_info(statement->as.label.label, pc));
            vector_push(result, statement);
            pc += statement->as.byte_def.size; 
        } else {
            PLUM_UNREACHABLE();
        }
    }

    for(int i = 0; i < vector_size(result); ++i) {
        rac0a_hl_statement_t* statement = vector_get(result, i);

        if(statement->type == RAC0A_HL_TYPE_CONSTVAL_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_CONSTBLOCK_DECL) {
        } else if(statement->type == RAC0A_HL_TYPE_LABEL) {
        } else if(statement->type == RAC0A_HL_TYPE_INSTRUCTION) {
            rac0a_hl_instruction_statement_t* hl_instruction = &statement->as.instruction;

            if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_NONE) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
            } else if(hl_instruction->value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
                rac0a_label_hl_info_t* info = rac0a_get_label_hl_info(&labels, hl_instruction->value.as.label);

                if(info == NULL) {
                    for(int i = 0; i < vector_size(&labels); ++i)
                        rac0a_free_label_hl_info(vector_get(&labels, i));
            
                    free_vector(&labels);
                    return rac0a_assemble_result_failed("Label is not defined", statement->pointer);
                } else {
                    hl_instruction->value.as.value = info->pointer;
                    hl_instruction->value.type = RAC0A_HL_VALUE_TYPE_NUMBER;
                }
            } else {
                PLUM_UNREACHABLE();
            }
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            rac0a_hl_word_def_statement_t* word_def = &statement->as.word_def;

            if(word_def->value.type == RAC0A_HL_VALUE_TYPE_NONE) {
            } else if(word_def->value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
            } else if(word_def->value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
            } else if(word_def->value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
                rac0a_label_hl_info_t* info = rac0a_get_label_hl_info(&labels, word_def->value.as.label);

                if(info == NULL) {
                    for(int i = 0; i < vector_size(&labels); ++i)
                        rac0a_free_label_hl_info(vector_get(&labels, i));
        
                    free_vector(&labels);
                    return rac0a_assemble_result_failed("Label is not defined", statement->pointer);
                } else {
                    word_def->value.as.value = info->pointer;
                    word_def->value.type = RAC0A_HL_VALUE_TYPE_NUMBER;
                }
            } else {
                PLUM_UNREACHABLE();
            }
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
        } else {
            PLUM_UNREACHABLE();
        }
    }

    for(int i = 0; i < vector_size(&labels); ++i)
        rac0a_free_label_hl_info(vector_get(&labels, i));

    free_vector(&labels);

    return rac0a_assemble_result_ok();
}

byte_vector_t rac0a_assemble_run_final_pass(rac0a_hl_statement_list_t* input) {
    byte_vector_t result;
    create_byte_vector(&result, 1000);

    for(int i = 0; i < vector_size(input); ++i) {
        rac0a_hl_statement_t* statement = vector_get(input, i);

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
                PLUM_UNREACHABLE();
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
                PLUM_UNREACHABLE();
            }

            rac0a_byte_vector_push_instruction(&result, instruction);
        } else if(statement->type == RAC0A_HL_TYPE_WORD_DEF) {
            rac0a_hl_word_def_statement_t word_def = statement->as.word_def;

            rac0_value_t value;

            if(word_def.value.type == RAC0A_HL_VALUE_TYPE_NONE) {
                // do nothing
            } else if(word_def.value.type == RAC0A_HL_VALUE_TYPE_CONSTVAL) {
                // do nothing
            } else if(word_def.value.type == RAC0A_HL_VALUE_TYPE_NUMBER) {
                value = word_def.value.as.value;
            } else if(word_def.value.type == RAC0A_HL_VALUE_TYPE_LABEL_POINTER) {
                // do nothing
            } else {
                PLUM_UNREACHABLE();
            }

            byte_vector_push64(&result, value);
        } else if(statement->type == RAC0A_HL_TYPE_BYTE_DEF) {
            for(int i = 0; i < statement->as.byte_def.size; ++i)
                byte_vector_push_byte(&result, statement->as.byte_def.array[i]);
        } else {
            PLUM_UNREACHABLE();
        }
    }

    return result;
}

rac0a_assemble_result_t rac0a_assemble_program(rac0a_compiler_configuration_t* configuration, byte_vector_t* result, rac0a_hl_statement_list_t* hl_statements) {
    {
        rac0a_assemble_result_t validation_result =  rac0a_assemble_run_validate_constval_multiple_pass(hl_statements);

        if(validation_result.code == RAC0A_OK) {
        } else if(validation_result.code == RAC0A_FAILED) {
            return validation_result;
        } else if(validation_result.code == RAC0A_ERROR) {
        }
    }

    {
        rac0a_assemble_result_t validation_result =  rac0a_assemble_run_validate_constblock_multiple_pass(hl_statements);

        if(validation_result.code == RAC0A_OK) {
        } else if(validation_result.code == RAC0A_FAILED) {
            return validation_result;
        } else if(validation_result.code == RAC0A_ERROR) {
        }
    }

    {
        rac0a_assemble_result_t validation_result =  rac0a_assemble_run_validate_const_multiple_pass(hl_statements);

        if(validation_result.code == RAC0A_OK) {
        } else if(validation_result.code == RAC0A_FAILED) {
            return validation_result;
        } else if(validation_result.code == RAC0A_ERROR) {
        }
    }

    {
        rac0a_assemble_result_t validation_result =  rac0a_assemble_run_validate_all_multiple_pass(hl_statements);

        if(validation_result.code == RAC0A_OK) {
        } else if(validation_result.code == RAC0A_FAILED) {
            return validation_result;
        } else if(validation_result.code == RAC0A_ERROR) {
        }
    }

    {
        rac0a_assemble_result_t validation_result =  rac0a_assemble_run_validate_label_multiple_pass(hl_statements);

        if(validation_result.code == RAC0A_OK) {
        } else if(validation_result.code == RAC0A_FAILED) {
            return validation_result;
        } else if(validation_result.code == RAC0A_ERROR) {
        }
    }
    
    {
        rac0a_assemble_result_t validation_result =  rac0a_assemble_run_validate_byte_def_multiple_pass(hl_statements);

        if(validation_result.code == RAC0A_OK) {
        } else if(validation_result.code == RAC0A_FAILED) {
            return validation_result;
        } else if(validation_result.code == RAC0A_ERROR) {
        }
    }

    {
        rac0a_assemble_result_t validation_result =  rac0a_assemble_run_validate_word_def_multiple_pass(hl_statements);

        if(validation_result.code == RAC0A_OK) {
        } else if(validation_result.code == RAC0A_FAILED) {
            return validation_result;
        } else if(validation_result.code == RAC0A_ERROR) {
        }
    }

    // first pass we collect all labels and constvalues
    rac0a_hl_statement_list_t pass1;
    create_vector(&pass1, 1000);
    rac0a_assemble_result_t parse_1_result =  rac0a_assemble_run_1_pass(&pass1, hl_statements);

    if(parse_1_result.code == RAC0A_OK) {
        // continue
    } else if(parse_1_result.code == RAC0A_FAILED) {
        return parse_1_result;
    } else if(parse_1_result.code == RAC0A_ERROR) {

    }

    if(configuration->dump)
        rac0a_log_hl_statements("1_assemble_pass.rac0a.dump.txt", &pass1);

    // then we calculate actual addresses and replace all label references
    rac0a_hl_statement_list_t pass2;
    create_vector(&pass2, 1000);
    rac0a_assemble_result_t parse_2_result = rac0a_assemble_run_2_pass(&pass2, &pass1);

    if(parse_2_result.code == RAC0A_OK) {
        // continue
    } else if(parse_2_result.code == RAC0A_FAILED) {
        return parse_2_result;
    } else if(parse_2_result.code == RAC0A_ERROR) {

    }

    if(configuration->dump)
        rac0a_log_hl_statements("2_assemble_pass.rac0a.dump.txt", &pass2);

    *result = rac0a_assemble_run_final_pass(&pass2);

    return (rac0a_assemble_result_t) { RAC0A_OK };
}
