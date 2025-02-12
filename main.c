#include <stdio.h>
#include <stdlib.h>

#include "rac0.h"

int main() {
    rac0_cpu_t cpu = (rac0_cpu_t) {
        .pc = 0,
        .halted = 0
    };

    rac0_inst_t insts[] = {
        (rac0_inst_t) { .opcode = 0x0FFF, .value = 0xFFFFFFFFFFFFFFF0 },
        (rac0_inst_t) { .opcode = 0x100F, .value = 0xFFFFFFFFFFFFFFF1 },
        (rac0_inst_t) { .opcode = 0x20F0, .value = 0xFFFFFFFFFFFFFFF2 },
    };

    rac0_memory_t ram = (rac0_memory_t) {
        // .memory = (rac0_byte_t*) malloc(sizeof(rac0_byte_t) * RAC0_MEGABYTE_SIZE)  
        .memory = (rac0_byte_t*) &insts
    };

    rac0_cpu_inst_cycle(&cpu, &ram);
    rac0_cpu_inst_cycle(&cpu, &ram);
    rac0_cpu_inst_cycle(&cpu, &ram);

    return 0;
}