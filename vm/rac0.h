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

#define RAC0_STATUS_MODE_BIT_MASK       ((rac0_value_t) (0b0000000000000000000000000000000000000000000000000000000000000001))
#define RAC0_STATUS_HALTED_BIT_MASK     ((rac0_value_t) (0b0000000000000000000000000000000000000000000000000000000000000010))
#define RAC0_STATUS_TIMER_MODE_BIT_MASK ((rac0_value_t) (0b0000000000000000000000000000000000000000000000000000000000000100))
#define RAC0_STATUS_PAGE_MODE_BIT_MASK  ((rac0_value_t) (0b0000000000000000000000000000000000000000000000000000000000001000))

typedef struct {
    // stack
    rac0_stack_t stack;

    // registers
    rac0_value_t pc;

    // idt
    rac0_value_t idt;
    rac0_value_t idts; // pointer to interupt table
    rac0_value_t iret; // interupt return pointer

    rac0_value_t status;
    rac0_value_t timer;
} rac0_cpu_t;

void rac0_set_status_bit(rac0_cpu_t* cpu, rac0_value_t mask, rac0_value_t bool_value);
rac0_value_t rac0_status_bit_is_set(rac0_cpu_t* cpu, rac0_value_t mask);

typedef struct {    
    rac0_byte_t* memory;

    rac0_value_t ptba;
    rac0_value_t pts;
    rac0_value_t ptps;
} rac0_memory_t;

typedef struct {
    rac0_device_t* devices;
    rac0_value_t device; 
    rac0_value_t devc; 
} rac0_device_selector_t;

typedef struct {
    rac0_cpu_t* cpu;
    rac0_memory_t* memory;
    rac0_device_selector_t* device_selector;
} rac0_vm_t;

rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory);
void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_device_selector_t* device_selector);

void rac0_vm_cycle(rac0_vm_t* vm);
rac0_value_t rac0_vm_halted(rac0_vm_t* vm);

#endif