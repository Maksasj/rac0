#ifndef DISK_PERIPHERAL_H
#define DISK_PERIPHERAL_H

#include "plum.h"
#include "rac0_base.h"
#include "rac0_utils.h"

typedef struct {
    rac0_value_t file_size;
    rac0_value_t file_data_pointer;
} disk_device_file_entry_t;

typedef struct {
    rac0_value_t protocol_type;
    rac0_value_t file_entry_count;
    disk_device_file_entry_t disk_device_file_entries[16];
} disk_device_d_data_t;

typedef struct {
    disk_device_d_data_t d;

    rac0_byte_t* data;

    // utils
    rac0_value_t load_file_offset;
}  disk_device_data_t;

void disk_device_load_file(disk_device_data_t* data, char* file_name);

void disk_device_push(void* device_data, rac0_u64_t address, rac0_value_t value);
rac0_value_t disk_device_pool(void* device_data, rac0_u64_t address);

#endif