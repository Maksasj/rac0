#ifndef RAC0_H
#define RAC0_H

#include <stdio.h>

#include "plum.h"
#include "rac0_base.h"

typedef struct {
    void* device_data;
    void (*push)(void* device_data, rac0_u64_t adress, rac0_value_t value);
    rac0_value_t (*pool)(void* device_data, rac0_u64_t adress);
} rac0_device_t;

typedef struct {
    rac0_value_t values[RAC0_MAX_STACK_SIZE];
    rac0_u64_t top;
} rac0_stack_t;

rac0_u8_t rac0_stack_empty(rac0_stack_t* stack);
void rac0_stack_push(rac0_stack_t* stack, rac0_value_t value);
rac0_value_t rac0_stack_get_top(rac0_stack_t* stack);
rac0_value_t rac0_stack_get_next(rac0_stack_t* stack);
void rac0_stack_drop(rac0_stack_t* stack);

typedef enum {
    RAC0_CPU_MODE_KERNEL = 0,
    RAC0_CPU_MODE_USER = 1
} rac0_cpu_mode_t;

typedef struct {
    // stack
    rac0_stack_t stack;

    // registers
    rac0_value_t pc;
    rac0_value_t device; 
    rac0_value_t halted;

    // idt
    rac0_value_t idt; // pointer to interupt table
    rac0_value_t iret; // interupt return pointer

    rac0_value_t mode; // mode bit
} rac0_cpu_t;

typedef struct {    
    rac0_byte_t* memory;
} rac0_memory_t;

rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory);
void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_device_t* devices);
rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory);
void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_device_t* devices);

#endif