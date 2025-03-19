#include "rac0_utils.h"
#include "rac0a_assembler.h"

int main(int argc, char *argv[]) {
    string_set_t set;
    create_string_set(&set, 1000);

    string_set_insert(&set, "test");

    PLUM_LOG(PLUM_EXPERIMENTAL)

    /*
    if(argc < 3) {
        PLUM_LOG(PLUM_ERROR, "File usage: %s <input file name> <output file name>", argv[0]);
        return 1;
    }

    char* source = rac0_utils_read_file_string(argv[1]);

    if(source == NULL)
        return 1;

    rac0a_hl_statement_list_t hl_statements = rac0a_parse_program(source);
    PLUM_LOG(PLUM_INFO, "Parsed program");
    PLUM_LOG(PLUM_INFO, "Parser generated %d hl statements", vector_size(&hl_statements));

    rac0a_log_hl_statements("0_parse_pass.rac0a.dump.txt", &hl_statements);

    byte_vector_t program = rac0a_assemble_program(&hl_statements);

    FILE *out_file = fopen(argv[2], "wb");
    fwrite(program.data, sizeof(rac0_byte_t), program.size, out_file);
    fclose(out_file);
    */

    return 0;
}