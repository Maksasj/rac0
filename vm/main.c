#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "haul/haul.h"
#include "rac0.h"
#include "rac0_utils.h"

#include "sdl_peripheral.h"
#include "debug_peripheral.h"
#include "disk_peripheral.h"

void rac0_print_help(char* filename) {
    printf("Usage: %s <input file> [options]\n", filename);
    printf("Options:\n");
    printf("    --help          Display this information.\n");
    printf("    -dumpm          Dump memory after virtual machine haltes\n");
    printf("    -dsdl           Enable SDL peripheral device\n");
    printf("    -ddeb           Enable debug peripheral device\n");
    printf("    -dfile          Enable file system device\n");
}

typedef struct {
    char* input_filename;
    rac0_value_t dump_memory;
    rac0_value_t sdl_device;
    rac0_value_t debug_console_device;
    rac0_value_t filesystem_device;
} rac0_vm_configuration_t;

int main(int argc, char *argv[]) {
    if(argc < 2) {
        rac0_print_help(argv[0]);
        return 1;
    }

    rac0_vm_configuration_t configuration;
    configuration.input_filename = argv[1];
    configuration.dump_memory = 0;
    configuration.sdl_device = 0;
    configuration.debug_console_device = 0;
    configuration.filesystem_device = 0;

    string_set_t arguments;
    arguments.items.items = (void**) argv;
    arguments.items.stored = argc;
    arguments.items.capacity = argc;

    if(string_set_has(&arguments, "--help")) {
        rac0_print_help(argv[1]);
        return 0;
    }

    if(string_set_has(&arguments, "-dumpm"))
        configuration.dump_memory = 1;

    if(string_set_has(&arguments, "-dsdl"))
        configuration.sdl_device = 1;

    if(string_set_has(&arguments, "-ddeb"))
        configuration.debug_console_device = 1;

    if(string_set_has(&arguments, "-dfile"))
        configuration.filesystem_device = 1;

    // cpu initialization
    rac0_cpu_t cpu = (rac0_cpu_t) {
        .stack = {
            .top = 0 
        },

        .pc = 0,
        .cycle = 0,

        .idt = 0,
        .idts = 0,
        .iret = {
            .top = 0
        },
    
        .status = 0
    };  

    // memory initialization
    rac0_memory_t memory = (rac0_memory_t) {
        .memory = (rac0_byte_t*) calloc(sizeof(rac0_byte_t) * RAC0_MEGABYTE_SIZE, 1),
        .memory_size = RAC0_MEGABYTE_SIZE,
        .ptba = 0x0,
        .pts = 0x0,
        .ptps = 0x0
    };

    rac0_byte_t* byte_code = (rac0_byte_t*) rac0_utils_read_file_string(configuration.input_filename);
    int byte_code_size = rac0_utils_read_file_size(configuration.input_filename);
        
    if(byte_code_size == -1)
        return 1;

    if(byte_code == NULL)
        return 1;

    memcpy(memory.memory, byte_code, byte_code_size);

    // sdl peripheral initialization
    sdl_peripheral_devices_data_t sdl_peripheral;
    pthread_t sdl_peripheral_thread;

    if(configuration.sdl_device) {
        sdl_peripheral_initialize(&sdl_peripheral, 160, 144);
        pthread_create(&sdl_peripheral_thread, NULL, sdl_peripheral_run, (void*) &sdl_peripheral);
    }
    
    vector_t devices;
    create_vector(&devices, 1000);

    if(configuration.debug_console_device) {
        rac0_device_t* device = (rac0_device_t*) malloc(sizeof(rac0_device_t));
        device->device_data = NULL;
        device->push = debug_console_device_push;
        device->pool = debug_console_device_pool;
        vector_push(&devices, device);
    }

    if(configuration.sdl_device) {
        rac0_device_t* screen = (rac0_device_t*) malloc(sizeof(rac0_device_t));
        screen->device_data = &sdl_peripheral,
        screen->push = sdl_peripheral_screen_device_push;
        screen->pool = sdl_peripheral_screen_device_pool;
        vector_push(&devices, screen);

        rac0_device_t* keyboard = (rac0_device_t*) malloc(sizeof(rac0_device_t));
        keyboard->device_data = &sdl_peripheral,
        keyboard->push = sdl_peripheral_keyboard_device_push;
        keyboard->pool = sdl_peripheral_keyboard_device_pool;
        vector_push(&devices, keyboard);
    }

    // filesystem device
    disk_device_data_t disk_device = (disk_device_data_t) {
        .d = {
            .protocol_type = RAC0OC_DEVICE_PROTOCOL_DISK,
            .file_entry_count = 0,
        },

        .data = (void*) malloc(RAC0_MEGABYTE_SIZE),

        // utils
        .load_file_offset = 0,
    };

    if(configuration.filesystem_device) {
        rac0_device_t* disk = (rac0_device_t*) malloc(sizeof(rac0_device_t));

        disk_device_load_file(&disk_device, "target/oc/p0.bin");
        disk_device_load_file(&disk_device, "target/oc/p1.bin");

        disk->device_data = &disk_device,

        disk->push = disk_device_push;
        disk->pool = disk_device_pool;
        vector_push(&devices, disk);
    }

    // device selector initialization
    rac0_device_selector_t device_selector = (rac0_device_selector_t) {
        .devices = (rac0_device_t**) devices.items,
        .device = 0,
        .devc = vector_size(&devices)
    };

    PLUM_LOG(PLUM_EXPERIMENTAL, "%d", device_selector.devc);

    // virtual machine initialization
    rac0_vm_t vm = (rac0_vm_t) {
        .cpu = &cpu,
        .memory = &memory,
        .device_selector = &device_selector
    };
    
    PLUM_LOG(PLUM_INFO, "Virtual machine started");

    while(!rac0_vm_halted(&vm))
        rac0_vm_cycle(&vm);  

    PLUM_LOG(PLUM_INFO, "Virtual machine finished");

    if(configuration.sdl_device) {
        pthread_join(sdl_peripheral_thread, NULL);
        sdl_peripheral_free(&sdl_peripheral);
    }

    if(configuration.filesystem_device)
        free(disk_device.data);

    if(configuration.dump_memory)
        rac0_dump_memory_fmt(vm.memory, "memory.txt");

    return 0;
}