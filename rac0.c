#include <stdio.h>
#include <stdlib.h>

#include "rac0.h"

void debug_console_device_push(void* device_data, rac0_u64_t adress, rac0_value_t value) {
    PLUM_LOG(PLUM_EXPERIMENTAL, "%d", value);
}

rac0_value_t debug_console_device_pool(void* device_data, rac0_u64_t adress) {
    return 0;
}

int main() {
    rac0_cpu_t cpu = (rac0_cpu_t) {
        .pc = 0,
        .device = 0,
        .halted = 0,
        .stack = {
            .top = 0 
        }
    };

    rac0_inst_t insts[] = {
        (rac0_inst_t) { .opcode = RAC0_PUSHA_OPCODE, .value = 0x0000000000000001 },
        (rac0_inst_t) { .opcode = RAC0_ADDAT_OPCODE, .value = 1 },
        (rac0_inst_t) { .opcode = RAC0_PUSHDA_OPCODE, .value = 0 },
        (rac0_inst_t) { .opcode = RAC0_JMPLUA_OPCODE, .value = 2 * sizeof(rac0_inst_t) },
        (rac0_inst_t) { .opcode = RAC0_HALT_OPCODE },
    };

    rac0_memory_t memory = (rac0_memory_t) {
        // .memory = (rac0_byte_t*) malloc(sizeof(rac0_byte_t) * RAC0_MEGABYTE_SIZE)  
        .memory = (rac0_byte_t*) &insts
    };

    rac0_device_t devices[] = {
        (rac0_device_t) { 
            .device_data = NULL, 
            .push = debug_console_device_push, 
            .pool = debug_console_device_pool 
        }
    };

    PLUM_LOG(PLUM_INFO, "CPU started");

    while(!cpu.halted)
        rac0_cpu_inst_cycle(&cpu, &memory, devices);

    PLUM_LOG(PLUM_INFO, "CPU halted");

    return 0;
}