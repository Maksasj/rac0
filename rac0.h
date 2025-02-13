#ifndef RAC0_H
#define RAC0_H

#include <stdio.h>

#define PLUM_IMPLEMENTATION
#include "plum.h"

#include "rac0_base.h"

typedef struct {
    void* device_data;
    void (*push)(void* device_data, rac0_u64_t adress, rac0_value_t value);
    rac0_value_t (*pool)(void* device_data, rac0_u64_t adress);
} rac0_device_t;

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
    rac0_u64_t device; 
    rac0_byte_t halted;
} rac0_cpu_t;

typedef struct {    
    rac0_byte_t* memory;
} rac0_memory_t;

rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory);
void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_device_t* devices);

rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory) {
    return *((rac0_inst_t*) &memory->memory[pc]);
}

void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_device_t* devices) {
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
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_stack_push(&cpu->stack, top);
        rac0_stack_push(&cpu->stack, next);
    } else if(inst.opcode == RAC0_ADD_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);

        rac0_stack_push(&cpu->stack, top + next);
        goto inc;
    } else if(inst.opcode == RAC0_STORE_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode STORE is not implemented");
    } else if(inst.opcode == RAC0_STOREA_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode STOREA is not implemented");
    } else if(inst.opcode == RAC0_LOAD_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode LOAD is not implemented");
    } else if(inst.opcode == RAC0_LOADA_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode LOADA is not implemented");
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
    } else if(inst.opcode == RAC0_SETDA_OPCODE) {
        cpu->device = inst.value;
        goto inc;
    } else if(inst.opcode == RAC0_SETDT_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        cpu->device = top;
        goto inc;
    } else if(inst.opcode == RAC0_POOLDA_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode POOLDA is not implemented");
    } else if(inst.opcode == RAC0_POOLDT_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode POOLDT is not implemented");
    } else if(inst.opcode == RAC0_PUSHDA_OPCODE) {
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_device_t device = devices[cpu->device];
        (*device.push)(device.device_data, inst.value, top);
        goto inc;
    } else if(inst.opcode == RAC0_PUSHDT_OPCODE) {
        PLUM_LOG(PLUM_ERROR, "Opcode PUSHDT is not implemented");
        rac0_value_t top = rac0_stack_get_top(&cpu->stack);
        rac0_value_t next = rac0_stack_get_next(&cpu->stack);
        rac0_device_t device = devices[cpu->device];
        (*device.push)(device.device_data, top, next);
    } else if(inst.opcode == RAC0_WAIT_OPCODE) {
        goto inc; // do nothing
    } else {
        PLUM_LOG(PLUM_ERROR, "Opcode is not implemented %.4x", inst.opcode);
        return;
    }

    inc:
    cpu->pc += sizeof(rac0_inst_t);

    cont:

    #ifdef RAC0_LOG_CPU_INSTRUCTION
    PLUM_LOG(PLUM_TRACE, "[ 0x%.4x ] 0x%.16llx %s", inst.opcode, inst.value, RAC0_OPCODE_STRING[inst.opcode]);
    #endif
}

#endif