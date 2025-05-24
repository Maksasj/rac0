#include "disk_peripheral.h"

void disk_device_load_file(disk_device_data_t* data, char* file_name) {
    rac0_byte_t* byte_code = (rac0_byte_t*) rac0_utils_read_file_string(file_name);
    int byte_code_size = rac0_utils_read_file_size(file_name);

    data->d.disk_device_file_entries[data->d.file_entry_count].file_size = byte_code_size;
    data->d.disk_device_file_entries[data->d.file_entry_count].file_data_pointer = data->load_file_offset;

    memcpy(&data->data[data->load_file_offset], byte_code, byte_code_size);
    
    data->load_file_offset += byte_code_size;
    ++data->d.file_entry_count;
}

void disk_device_push(void* device_data, rac0_u64_t address, rac0_value_t value) {
    disk_device_data_t* disk = (disk_device_data_t*) device_data;  

    if(address >= 0 && (address < sizeof(disk_device_d_data_t))) {
        ((rac0_byte_t*) disk)[address] = value;
    } else {
        ((rac0_byte_t*) disk->data)[address - sizeof(disk_device_d_data_t)] = value;
    }
}

rac0_value_t disk_device_pool(void* device_data, rac0_u64_t address) {
    disk_device_data_t* disk = (disk_device_data_t*) device_data;  

    if(address >= 0 && (address < sizeof(disk_device_d_data_t))) {
        return ((rac0_byte_t*) disk)[address];
    }

    PLUM_LOG(PLUM_EXPERIMENTAL, "DATA %d", disk->data[address]);

    return disk->data[address - sizeof(disk_device_d_data_t)];
}
