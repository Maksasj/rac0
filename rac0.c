#include <stdio.h>
#include <stdlib.h>

#include "rac0.h"

int main() {
    rac0_cpu_t cpu = (rac0_cpu_t) {
        .pc = 0,
        .halted = 0,
        .stack = {
            .top = 0 
        }
    };

    rac0_inst_t insts[] = {
        (rac0_inst_t) { .opcode = RAC0_PUSHA_OPCODE, .value = 0x0000000000000001 },
        (rac0_inst_t) { .opcode = RAC0_ADDAT_OPCODE, .value = 1 },
        (rac0_inst_t) { .opcode = RAC0_JMPLUA_OPCODE, .value = 1 * sizeof(rac0_inst_t) },
        (rac0_inst_t) { .opcode = RAC0_HALT_OPCODE },
    };

    rac0_memory_t ram = (rac0_memory_t) {
        // .memory = (rac0_byte_t*) malloc(sizeof(rac0_byte_t) * RAC0_MEGABYTE_SIZE)  
        .memory = (rac0_byte_t*) &insts
    };

    PLUM_LOG(PLUM_INFO, "CPU started");

    while(!cpu.halted)
        rac0_cpu_inst_cycle(&cpu, &ram);

    PLUM_LOG(PLUM_INFO, "CPU halted");

    return 0;
}