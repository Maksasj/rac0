#ifndef RAC0_BASE_H
#define RAC0_BASE_H

#include "plum.h"

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

#define RAC0_HALT_OPCODE        ((rac0_opcode_t) 0)

#define RAC0_PUSHA_OPCODE       ((rac0_opcode_t) 1)
#define RAC0_PUSHPC_OPCODE      ((rac0_opcode_t) 2)
#define RAC0_DUPT_OPCODE        ((rac0_opcode_t) 3)
#define RAC0_DUPN_OPCODE        ((rac0_opcode_t) 4)
#define RAC0_DROP_OPCODE        ((rac0_opcode_t) 5)
#define RAC0_SWAP_OPCODE        ((rac0_opcode_t) 6)

#define RAC0_STORE_OPCODE       ((rac0_opcode_t) 7)
#define RAC0_STOREA_OPCODE      ((rac0_opcode_t) 8)
#define RAC0_LOAD_OPCODE        ((rac0_opcode_t) 9)
#define RAC0_LOADA_OPCODE       ((rac0_opcode_t) 10)

#define RAC0_ADD_OPCODE         ((rac0_opcode_t) 11)
#define RAC0_ADDC_OPCODE        ((rac0_opcode_t) 12)
#define RAC0_ADDAT_OPCODE       ((rac0_opcode_t) 13)
#define RAC0_ADDAN_OPCODE       ((rac0_opcode_t) 14)

#define RAC0_JMPGA_OPCODE       ((rac0_opcode_t) 15)

#define RAC0_SETDA_OPCODE       ((rac0_opcode_t) 16)
#define RAC0_SETDT_OPCODE       ((rac0_opcode_t) 17)
#define RAC0_POOLDA_OPCODE      ((rac0_opcode_t) 18)
#define RAC0_POOLDT_OPCODE      ((rac0_opcode_t) 19)
#define RAC0_PUSHDA_OPCODE      ((rac0_opcode_t) 20)
#define RAC0_PUSHDT_OPCODE      ((rac0_opcode_t) 21)

#define RAC0_WAIT_OPCODE        ((rac0_opcode_t) 22)

typedef struct {
    rac0_opcode_t opcode;   // 16 bits | 2 bytes
    rac0_value_t value;     // 64 bits | 8 bytes
} rac0_inst_t;              // 80 bits | 10 bytes

static char* RAC0_OPCODE_STRING[] = {
    "HALT",

    "PUSHA",
    "PUSHPC",
    "DUPT",
    "DUPN",
    "DROP",
    "SWAP",

    "STORE",
    "STOREA",
    "LOAD",
    "LOADA",

    "ADD",
    "ADDC",
    "ADDAT",
    "ADDAN",

    "JMPGA",

    "SETDA",
    "SETDT",
    "POOLDA",
    "POOLDT",
    "PUSHDA",
    "PUSHDT",

    "WAIT"
};

#endif