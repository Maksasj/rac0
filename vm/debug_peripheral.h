#ifndef DEBUG_PERIPHERAL_H
#define DEBUG_PERIPHERAL_H

#include "plum.h"
#include "rac0_base.h"

typedef struct {

}  debug_peripheral_device_data_t;

void debug_console_device_push(void* device_data, rac0_u64_t address, rac0_value_t value);
rac0_value_t debug_console_device_pool(void* device_data, rac0_u64_t address);

#endif