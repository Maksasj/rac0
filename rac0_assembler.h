#ifndef RAC0_ASSEMBLER_H
#define RAC0_ASSEMBLER_H

#include "rac0_parser.h"

void rac0a_assemble_program(rac0a_program_t* program) {
    int size = vector_size(&program->hl_statements);
    
    for(int i = 0; i < size; ++i) {
        rac0a_hl_statement_t* statement = vector_get(&program->hl_statements, i);

        PLUM_LOG(PLUM_DEBUG, "%d hl: [ %s ]", i, RAC0A_HL_STRING[statement->type]);
    }
}

#endif
