#include "debug_peripheral.h"

void debug_console_device_push(void* device_data, rac0_u64_t address, rac0_value_t value) {
    PLUM_LOG(PLUM_EXPERIMENTAL, "%d", value);
}

rac0_value_t debug_console_device_pool(void* device_data, rac0_u64_t address) {
    return 0;
}
