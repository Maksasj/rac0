#ifndef RAC0_H
#define RAC0_H

#include <stdio.h>

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

typedef struct {
    rac0_opcode_t opcode;   // 16 bits | 2 bytes
    rac0_value_t value;     // 64 bits | 8 bytes
} rac0_inst_t;              // 80 bits | 10 bytes

typedef struct {
    rac0_value_t values[RAC0_MAX_STACK_SIZE];
    rac0_u64_t top;
} rac0_stack_t;

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

    printf("[ 0x%.4x ] 0x%.16lx\n", inst.opcode, inst.value);

    cpu->pc += sizeof(rac0_inst_t);
}

#endif