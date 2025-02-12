#ifndef RAC0_H
#define RAC0_H

#include <stdio.h>

#define PLUM_IMPLEMENTATION
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

#define RAC0_ADD_OPCODE         ((rac0_opcode_t) 7)
#define RAC0_ADDC_OPCODE        ((rac0_opcode_t) 8)
#define RAC0_ADDAT_OPCODE       ((rac0_opcode_t) 9)
#define RAC0_ADDAN_OPCODE       ((rac0_opcode_t) 10)

#define RAC0_JMPGA_OPCODE       ((rac0_opcode_t) 11)
#define RAC0_JMPLUA_OPCODE       ((rac0_opcode_t) 12)
#define RAC0_JMPLDA_OPCODE       ((rac0_opcode_t) 13)

#define RAC0_WAIT_OPCODE        ((rac0_opcode_t) 14)

static char* OPCODE_STRING[] = {
    "HALT",

    "PUSHA",
    "PUSHPC",
    "DUPT",
    "DUPN",
    "DROP",
    "SWAP",

    "ADD",
    "ADDC",
    "ADDAT",
    "ADDAN",

    "JMPGA",
    "JMPLUA",
    "JMPLDA",

    "WAIT"
};

typedef struct {
    rac0_opcode_t opcode;   // 16 bits | 2 bytes
    rac0_value_t value;     // 64 bits | 8 bytes
} rac0_inst_t;              // 80 bits | 10 bytes

typedef struct {
    rac0_value_t values[RAC0_MAX_STACK_SIZE];
    rac0_u64_t top;
} rac0_stack_t;

rac0_u8_t rac0_stack_empty(rac0_stack_t* stack) {
    return stack->top == 0;
}

void rac0_stack_push(rac0_stack_t* stack, rac0_value_t value) {
    stack->values[stack->top] = value;
    ++stack->top;
}

rac0_value_t rac0_stack_get_top(rac0_stack_t* stack) {
    if(rac0_stack_empty(stack))
        return 0;

    return stack->values[stack->top - 1];
}

rac0_value_t rac0_stack_get_next(rac0_stack_t* stack) {
    if(rac0_stack_empty(stack))
        return 0;

    if(stack->top <= 1)
        return 0;

    return stack->values[stack->top - 2];
}

void rac0_stack_drop(rac0_stack_t* stack) {
    if(rac0_stack_empty(stack))
        return;

    --stack->top;
}

typedef struct {
    rac0_stack_t stack;
    rac0_u64_t pc; 
    rac0_byte_t halted;
} rac0_cpu_t;

typedef struct {    
    rac0_byte_t* memory;
} rac0_memory_t;

rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory);
void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory);

rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory) {
    return *((rac0_inst_t*) &memory->memory[pc]);
}

void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory) {
    rac0_inst_t inst = rac0_fetch_inst(cpu->pc, memory);

    if(inst.opcode == RAC0_HALT_OPCODE) {
        cpu->halted = 1;
        goto inc;
    } else if(inst.opcode == RAC0_PUSHA_OPCODE) {
        rac0_stack_push(&cpu->stack, inst.value);
        goto inc;
    } else if(inst.opcode == RAC0_PUSHPC_OPCODE) {
        rac0_stack_push(&cpu->stack, cpu->pc);
        goto inc;
    } else if(inst.opcode == RAC0_DUPT_OPCODE) {
        rac0_stack_push(&cpu->stack, rac0_stack_get_top(&cpu->stack));
        goto inc;
    } else if(inst.opcode == RAC0_DUPN_OPCODE) {
        rac0_stack_push(&cpu->stack, rac0_stack_get_next(&cpu->stack));
        goto inc;
    } else if(inst.opcode == RAC0_DROP_OPCODE) {
        rac0_stack_drop(&cpu->stack);
        goto inc;
    } else if(inst.opcode == RAC0_SWAP_OPCODE) {
        // todo implement
    } else if(inst.opcode == RAC0_ADD_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);

        rac0_stack_push(&cpu->stack, top + next);
        goto inc;
    } else if(inst.opcode == RAC0_ADDC_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);

        rac0_stack_drop(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        
        rac0_stack_push(&cpu->stack, top + next);
        goto inc;
    } else if(inst.opcode == RAC0_ADDAT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);

        rac0_stack_drop(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        
        rac0_stack_push(&cpu->stack, next);
        rac0_stack_push(&cpu->stack, top + inst.value);
        goto inc;
    } else if(inst.opcode == RAC0_ADDAN_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);

        rac0_stack_drop(&cpu->stack);
        rac0_stack_drop(&cpu->stack);
        
        rac0_stack_push(&cpu->stack, next + inst.value);
        rac0_stack_push(&cpu->stack, top);
        goto inc;
    } else if(inst.opcode == RAC0_JMPGA_OPCODE) {
        cpu->pc = inst.value;
        goto cont;
    } else if(inst.opcode == RAC0_JMPLUA_OPCODE) {
        cpu->pc -= inst.value;
        goto cont;
    } else if(inst.opcode == RAC0_JMPLDA_OPCODE) {
        cpu->pc += inst.value;
        goto cont;
    } else if(inst.opcode == RAC0_WAIT_OPCODE) {
        goto inc; // do nothing
    } else {
        PLUM_LOG(PLUM_ERROR, "Opcode is not implemented %.4x", inst.opcode);
        return;
    }

    inc:
    cpu->pc += sizeof(rac0_inst_t);

    cont:

    PLUM_LOG(PLUM_TRACE, "[ 0x%.4x ] 0x%.16llx %s", inst.opcode, inst.value, OPCODE_STRING[inst.opcode]);
}

#endif