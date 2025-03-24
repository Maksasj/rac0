#ifndef RAC0A_PARSER_H
#define RAC0A_PARSER_H

#include "haul/haul.h"

#include "rac0_utils.h"

#include "plum.h"
#include "rac0a_lexer.h"
#include "rac0a_hl.h"

typedef struct {
    rac0a_result_code_t code;

    union {
        struct {
            const char* message;
            rac0_u64_t pointer;
        } error;
        struct {

        } ok;
    } as;
} rac0a_parse_result_t;

rac0a_parse_result_t rac0a_parse_result_ok();
rac0a_parse_result_t rac0a_parse_result_failed();
rac0a_parse_result_t rac0a_parse_result_error(const char* message, rac0_u64_t pointer);

typedef struct {
    rac0a_lexer_t lexer;
    rac0a_hl_statement_list_t hl_statements;
} rac0a_parser_t;

// parsing utils
rac0a_parse_result_t rac0a_parse_token(rac0a_parser_t* parser, rac0a_token_type_t type, rac0a_token_t* ret);
rac0a_parse_result_t rac0a_parse_exact_word(rac0a_parser_t* parser, const string_t lexem);

// parsing shortcuts
rac0a_parse_result_t rac0a_parse_at(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_dollar(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_l_paren(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_r_paren(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_colon(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_label(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_number(rac0a_parser_t* parser, rac0_value_t* number);
rac0a_parse_result_t rac0a_parse_l_bracket(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_r_bracket(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_percent(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_string(rac0a_parser_t* parser, string_t* string);
rac0a_parse_result_t rac0a_parse_ampersand(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_comment(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_comma(rac0a_parser_t* parser);
rac0a_parse_result_t rac0a_parse_eof(rac0a_parser_t* parser);

// other
rac0a_parse_result_t rac0a_parse_include_statement(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_constblock_definition(rac0a_parser_t* parser, rac0a_hl_constblock_statement_t* block);

rac0a_parse_result_t rac0a_parse_constval_definition(rac0a_parser_t* parser, rac0a_hl_constval_statement_t* constval);

rac0a_parse_result_t rac0a_parse_label_definition(rac0a_parser_t* parser, rac0a_hl_label_statement_t* label);
rac0a_parse_result_t rac0a_parse_const_thing_usage(rac0a_parser_t* parser, string_t* label);
rac0a_parse_result_t rac0a_parse_label_pointer(rac0a_parser_t* parser, string_t* label);
rac0a_parse_result_t rac0a_parse_value(rac0a_parser_t* parser, rac0a_hl_value_t* value);

rac0a_parse_result_t rac0a_parse_instruction_noarg(rac0a_parser_t* parser, const string_t lexem);

rac0a_parse_result_t rac0a_parse_instruction_arg(rac0a_parser_t* parser, const string_t lexem, rac0a_hl_value_t* value);

rac0a_parse_result_t rac0a_parse_instruction(rac0a_parser_t* parser, rac0a_hl_instruction_statement_t* inst);

rac0a_parse_result_t rac0a_parse_byte_definition(rac0a_parser_t* parser, rac0a_hl_byte_def_statement_t* value);

rac0a_parse_result_t rac0a_parse_constblock_usage(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_word_definition(rac0a_parser_t* parser, rac0a_hl_word_def_statement_t* value);

rac0a_parse_result_t rac0a_parse_statement_list(rac0a_parser_t* parser, vector_t* list);

rac0a_parse_result_t rac0a_parse_module_definition(rac0a_parser_t* parser);

rac0a_parse_result_t rac0a_parse_program(rac0a_parser_t* parser);

#endif
