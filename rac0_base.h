#ifndef RAC0_BASE_H
#define RAC0_BASE_H

typedef unsigned long long rac0_u64_t;
typedef unsigned long rac0_u32_t;
typedef unsigned short rac0_u16_t;
typedef unsigned char rac0_u8_t;

typedef signed long long rac0_i64_t;
typedef signed long rac0_i32_t;
typedef signed short rac0_i16_t;
typedef signed char rac0_i8_t;

#define RAC0_BYTE_SIZE          ((rac0_u64_t) 1)
#define RAC0_KILOBYTE_SIZE      ((rac0_u64_t )(1024 * RAC0_BYTE_SIZE))
#define RAC0_MEGABYTE_SIZE      ((rac0_u64_t )(1024 * RAC0_KILOBYTE_SIZE))

#define RAC0_MAX_STACK_SIZE 1024

typedef rac0_u8_t rac0_byte_t;

typedef rac0_u64_t rac0_value_t;
typedef rac0_u16_t rac0_opcode_t;

#define RAC0_HALT_OPCODE        ((rac0_opcode_t) 1) // cpu
#define RAC0_WAIT_OPCODE        ((rac0_opcode_t) 2)
#define RAC0_SETIDTT_OPCODE     ((rac0_opcode_t) 3)
#define RAC0_SETIDTST_OPCODE    ((rac0_opcode_t) 4)
#define RAC0_SETPTBAT_OPCODE    ((rac0_opcode_t) 5)
#define RAC0_SETPTST_OPCODE     ((rac0_opcode_t) 6)
#define RAC0_SETPTPST_OPCODE    ((rac0_opcode_t) 7)
#define RAC0_SETTT_OPCODE       ((rac0_opcode_t) 8)
#define RAC0_SETIRETT_OPCODE    ((rac0_opcode_t) 50) // todo
#define RAC0_SETSTT_OPCODE      ((rac0_opcode_t) 9)
#define RAC0_PUSHA_OPCODE       ((rac0_opcode_t) 10) // stack
#define RAC0_PUSHPC_OPCODE      ((rac0_opcode_t) 11)
#define RAC0_PUSHSS_OPCODE      ((rac0_opcode_t) 12)
#define RAC0_PUSHDC_OPCODE      ((rac0_opcode_t) 13)
#define RAC0_PUSHMS_OPCODE      ((rac0_opcode_t) 14)
#define RAC0_PUSHIRET_OPCODE    ((rac0_opcode_t) 51) // todo
#define RAC0_DUPT_OPCODE        ((rac0_opcode_t) 15)
#define RAC0_DUPN_OPCODE        ((rac0_opcode_t) 16)
#define RAC0_DROP_OPCODE        ((rac0_opcode_t) 17)
#define RAC0_SWAP_OPCODE        ((rac0_opcode_t) 18)
#define RAC0_STORE_OPCODE       ((rac0_opcode_t) 19) // memory
#define RAC0_STOREARAC_OPCODE   ((rac0_opcode_t) 54) 
#define RAC0_STOREA_OPCODE      ((rac0_opcode_t) 20)
#define RAC0_LOAD_OPCODE        ((rac0_opcode_t) 21)
#define RAC0_LOADA_OPCODE       ((rac0_opcode_t) 22)
#define RAC0_LOADARAC_OPCODE    ((rac0_opcode_t) 53)
#define RAC0_ADD_OPCODE         ((rac0_opcode_t) 23) // arithmetic
#define RAC0_SUB_OPCODE         ((rac0_opcode_t) 24)
#define RAC0_MUL_OPCODE         ((rac0_opcode_t) 25)
#define RAC0_DIV_OPCODE         ((rac0_opcode_t) 26)
#define RAC0_MOD_OPCODE         ((rac0_opcode_t) 27)
#define RAC0_CMP_OPCODE         ((rac0_opcode_t) 28) // logic
#define RAC0_NEG_OPCODE         ((rac0_opcode_t) 29)
#define RAC0_NOT_OPCODE         ((rac0_opcode_t) 30)
#define RAC0_AND_OPCODE         ((rac0_opcode_t) 31)
#define RAC0_OR_OPCODE          ((rac0_opcode_t) 32)
#define RAC0_NAND_OPCODE        ((rac0_opcode_t) 33)
#define RAC0_NOR_OPCODE         ((rac0_opcode_t) 34)
#define RAC0_XOR_OPCODE         ((rac0_opcode_t) 35)
#define RAC0_JMPA_OPCODE        ((rac0_opcode_t) 36) // flow
#define RAC0_JMPT_OPCODE        ((rac0_opcode_t) 37)
#define RAC0_JMPTC_OPCODE       ((rac0_opcode_t) 52)
#define RAC0_JZA_OPCODE         ((rac0_opcode_t) 38)
#define RAC0_JNZA_OPCODE        ((rac0_opcode_t) 39)
#define RAC0_JNEGA_OPCODE       ((rac0_opcode_t) 40)
#define RAC0_JPOSA_OPCODE       ((rac0_opcode_t) 41)
#define RAC0_SETDA_OPCODE       ((rac0_opcode_t) 42) // device
#define RAC0_SETDT_OPCODE       ((rac0_opcode_t) 43)
#define RAC0_FETCHDA_OPCODE     ((rac0_opcode_t) 44)
#define RAC0_FETCHDT_OPCODE     ((rac0_opcode_t) 45)
#define RAC0_PUTDA_OPCODE       ((rac0_opcode_t) 46)
#define RAC0_PUTDT_OPCODE       ((rac0_opcode_t) 47)
#define RAC0_INT_OPCODE         ((rac0_opcode_t) 48) // interrupt
#define RAC0_IRETC_OPCODE        ((rac0_opcode_t) 49)

rac0_value_t rac0_opcode_valid(rac0_opcode_t opcode);

rac0_value_t rac0_opcode_privileged(rac0_opcode_t opcode);

typedef struct __attribute__((packed)) {
    rac0_opcode_t opcode;   // 16 bits | 2 bytes
    rac0_value_t value;     // 64 bits | 8 bytes
} rac0_inst_t;              // 80 bits | 10 bytes

static char* RAC0_OPCODE_STRING[] = {
    "Wrong opcode",

    // cpu
    "HALT",
    "WAIT",
    "SETIDTT",
    "SETIDTST",
    "SETPTBAT",
    "SETPTST",
    "SETPTPST",
    "SETTT",
    "SETSTT",
    
    // stack
    "PUSHA",
    "PUSHPC",
    "PUSHSS",
    "PUSHDC",
    "PUSHMS",
    "DUPT",
    "DUPN",
    "DROP",
    "SWAP",
    
    // memory
    "STORE",
    "STOREA",
    "LOAD",
    "LOADA",
    
    // arithmetic
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    
    // logic
    "CMP",
    "NEG",
    "NOT",
    "AND",
    "OR",
    "NAND",
    "NOR",
    "XOR",
    
    // flow
    "JMPA",
    "JMPT",
    "JZA",
    "JNZA",
    "JNEGA",
    "JPOSA",
    
    // device
    "SETDA",
    "SETDT",
    "FETCHDA",
    "FETCHDT",
    "PUTDA",
    "PUTDT",
    
    // interrupt
    "INT",
    "IRET",

    "SETIRETT",
    "PUSHIRET",
    "JMPTC",
    "LOADARAC",
    "STOREARAC",
};

#endif