#include <stdio.h>
#include <stdlib.h>

#include "rac0.h"

#include "rac0_utils.h"

void debug_console_device_push(void* device_data, rac0_u64_t adress, rac0_value_t value) {
    PLUM_LOG(PLUM_EXPERIMENTAL, "%d", value);
}

rac0_value_t debug_console_device_pool(void* device_data, rac0_u64_t adress) {
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        PLUM_LOG(PLUM_ERROR, "File usage: %s <input file name>", argv[0]);
        return 1;
    }

    rac0_cpu_t cpu = (rac0_cpu_t) {
        .pc = 0,
        .device = 0,
        .halted = 0,
        .stack = {
            .top = 0 
        }
    };  

    rac0_memory_t memory = (rac0_memory_t) {
        .memory = (rac0_byte_t*) calloc(sizeof(rac0_byte_t) * RAC0_MEGABYTE_SIZE, 1)  
    };

    rac0_byte_t* byte_code = (rac0_byte_t*) rac0_utils_read_file_string(argv[1]);
    
    if(byte_code == NULL)
        return 1;

    size_t byte_code_size = strlen(byte_code);
    memcpy(memory.memory, byte_code, byte_code_size);

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