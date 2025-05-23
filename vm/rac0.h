#ifndef RAC0_H
#define RAC0_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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

#define RAC0_USER_MODE                  ((rac0_value_t) (0b1))
#define RAC0_KERNEL_MODE                ((rac0_value_t) (0b0))

typedef struct {
    // stack
    rac0_stack_t stack;

    // registers
    rac0_value_t pc;
    rac0_value_t cycle;

    // idt
    rac0_value_t idt; // pointer to interupt table
    rac0_value_t idts;
    rac0_stack_t iret; // interupt return pointer

    rac0_value_t status;
    rac0_value_t timer;
} rac0_cpu_t;

void rac0_set_status_bit(rac0_cpu_t* cpu, rac0_value_t mask, rac0_value_t bool_value);
rac0_value_t rac0_status_bit_is_set(rac0_cpu_t* cpu, rac0_value_t mask);

typedef struct {    
    rac0_byte_t* memory;
    rac0_value_t memory_size;

    rac0_value_t ptba;
    rac0_value_t pts;
    rac0_value_t ptps;
} rac0_memory_t;

rac0_value_t rac0_valid_memory_access(rac0_memory_t* memory, rac0_value_t virtual_address, rac0_value_t paging);
rac0_value_t rac0_get_physical_address(rac0_memory_t* memory, rac0_value_t virtual_address, rac0_value_t paging);
void rac0_dump_memory_fmt(rac0_memory_t* memory, const char* file_name);

typedef struct {
    rac0_device_t** devices;
    rac0_value_t device; 
    rac0_value_t devc; 
} rac0_device_selector_t;

typedef struct {
    rac0_cpu_t* cpu;
    rac0_memory_t* memory;
    rac0_device_selector_t* device_selector;
} rac0_vm_t;

#define RAC0_INTERRUPT_TIMER                        ((rac0_value_t) (0x0))
#define RAC0_INTERRUPT_INVINST                      ((rac0_value_t) (0x1))
#define RAC0_INTERRUPT_PRIV                         ((rac0_value_t) (0x2))
#define RAC0_INTERRUPT_INVINT                       ((rac0_value_t) (0x3))
#define RAC0_INTERRUPT_INVPACC                      ((rac0_value_t) (0x4))
#define RAC0_INTERRUPT_INVDEV                       ((rac0_value_t) (0x5))
#define RAC0_INTERRUPT_DEVAMCH                      ((rac0_value_t) (0x6))

void rac0_cpu_throw_interrupt(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_value_t code);
void rac0_cpu_throw_interrupt_step(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_value_t code);
rac0_inst_t rac0_fetch_inst(rac0_u64_t pc, rac0_memory_t* memory);
void rac0_cpu_inst_cycle(rac0_cpu_t* cpu, rac0_memory_t* memory, rac0_device_selector_t* device_selector);

void rac0_vm_cycle(rac0_vm_t* vm);
rac0_value_t rac0_vm_halted(rac0_vm_t* vm);

#endif