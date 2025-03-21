#include "rac0_utils.h"
#include "rac0a_assembler.h"

typedef struct {
    char* input_filename;
    char* output_filename;
    rac0_value_t dump;
} rac0a_compiler_configuration_t;

void rac0a_print_help(char* filename) {
    printf("Usage: %s [options] <input file> <output file>\n", filename);
    printf("Options:\n");
    printf("    --help          Display this information.");
    printf("    -dump           Dump parsing and assembly stage results");
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        rac0a_print_help( argv[0]);
        return 1;
    }

    rac0a_compiler_configuration_t configuration;
    configuration.input_filename = argv[1];
    configuration.output_filename = argv[2];

    string_set_t arguments;
    arguments.items.items = (void**) argv;
    arguments.items.stored = argc;
    arguments.items.capacity = argc;

    if(string_set_has(&arguments, "--help")) {
        rac0a_print_help( argv[0]);
        return 0;
    }

    if(string_set_has(&arguments, "-dump"))
        PLUM_LOG(PLUM_EXPERIMENTAL, "Dump logs");

    char* source = rac0_utils_read_file_string(configuration.input_filename);

    if(source == NULL) {
        printf("fatal error: %s: No such file or directory\n", configuration.input_filename);
        return 1;
    }

    // Parse program
    rac0a_parser_t parser = {
        .lexer = (rac0a_lexer_t) {
            .input = source,
            .pointer = 0
        }
    };
    create_vector(&parser.hl_statements, 1024);

    rac0a_parse_result_t parse_result = rac0a_parse_program(&parser);

    if(parse_result.code == RAC0A_OK) {
        
    } else if(parse_result.code == RAC0A_FAILED) {
        rac0_value_t pointer = parse_result.as.error.pointer;

        rac0_value_t token_start = pointer;
        while (isspace(source[token_start]))
            token_start++;

        rac0_value_t line = rac0a_get_line_number_from_index(source, pointer) + 2;
        rac0_value_t column = 0;
        rac0_value_t line_length = rac0a_get_line_length(source + token_start); 

        printf("%s:%llu:%llu: \x1B[31merror\x1B[0m: %s\n", configuration.input_filename, line, column, parse_result.as.error.message);

        printf("   %llu | \x1B[31m", line);

        for(int i = 0; i < line_length; ++i)
            printf("%c", source[token_start + i]);

        printf("\x1B[0m\n");

        printf("   %llu | \x1B[31m^", line + 1);

        for(int i = 0; i < line_length - 1; ++i)
            printf("~");

        printf("\x1B[0m\n");
    } else if(parse_result.code == RAC0A_ERROR) {
        
    }

    rac0a_log_hl_statements("0_parse_pass.rac0a.dump.txt", &parser.hl_statements);

    // Assemble program
    byte_vector_t program;
    create_byte_vector(&program, 1000);

    rac0a_assemble_result_t assemble_result = rac0a_assemble_program(&program, &parser.hl_statements);

    // Save program
    FILE *out_file = fopen(configuration.output_filename, "wb");
    fwrite(program.data, sizeof(rac0_byte_t), program.size, out_file);
    fclose(out_file);

    return 0;
}