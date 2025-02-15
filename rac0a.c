#include "rac0a_utils.h"
#include "rac0a_assembler.h"

int main(int argc, char *argv[]) {
    if(argc < 2) {
        PLUM_LOG(PLUM_ERROR, "File usage: %s <input file name>", argv[0]);
        return 1;
    }

    char* source = rac0_utils_read_file_string(argv[1]);

    if(source == NULL)
        return 1;

    rac0a_program_t program;

    rac0a_parse_program(&program, source);
    PLUM_LOG(PLUM_INFO, "Parsed program");

    PLUM_LOG(PLUM_INFO, "Parser generated %d hl statements", vector_size(&program.hl_statements));

    rac0a_assemble_program(&program);

    return 0;
}