#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "rac0.h"

#include "rac0_utils.h"
#include "sdl_peripheral.h"

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
        .stack = {
            .top = 0 
        },

        .pc = 0,
        .device = 0,

        .idt = 0,
        .idts = 0,
        .iret = 0,
    
        .status = 0
    };  

    rac0_memory_t memory = (rac0_memory_t) {
        .memory = (rac0_byte_t*) calloc(sizeof(rac0_byte_t) * RAC0_MEGABYTE_SIZE, 1),
        .ptba = 0x0,
        .pts = 0x0,
        .ptps = 0x0
    };

    rac0_byte_t* byte_code = (rac0_byte_t*) rac0_utils_read_file_string(argv[1]);
    int byte_code_size = rac0_utils_read_file_size(argv[1]);
        
    if(byte_code_size == -1)
        return 1;

    if(byte_code == NULL)
        return 1;

    memcpy(memory.memory, byte_code, byte_code_size);

    sdl_peripheral_devices_data_t sdl_peripheral;
    sdl_peripheral_initialize(&sdl_peripheral, 160, 144);

    pthread_t sdl_peripheral_thread;
    pthread_create(&sdl_peripheral_thread, NULL, sdl_peripheral_run, (void*) &sdl_peripheral);

    rac0_device_t devices[] = {
        (rac0_device_t) { // Debug console
            .device_data = NULL, 
            .push = debug_console_device_push, 
            .pool = debug_console_device_pool 
        },
        (rac0_device_t) { // SDL screen
            .device_data = &sdl_peripheral, 
            .push = sdl_peripheral_screen_device_push, 
            .pool = sdl_peripheral_screen_device_pool 
        },
        (rac0_device_t) { // SDL keyboard
            .device_data = &sdl_peripheral, 
            .push = sdl_peripheral_keyboard_device_push, 
            .pool = sdl_peripheral_keyboard_device_pool 
        }
    };

    PLUM_LOG(PLUM_INFO, "CPU started");

    while(!rac0_status_bit_is_set(&cpu, RAC0_STATUS_HALTED_BIT_MASK))
        rac0_cpu_inst_cycle(&cpu, &memory, devices);

    PLUM_LOG(PLUM_INFO, "CPU halted");

    pthread_join(sdl_peripheral_thread, NULL);
    sdl_peripheral_free(&sdl_peripheral);

    return 0;
}