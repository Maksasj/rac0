#ifndef SDL_PERIPHERAL_H
#define SDL_PERIPHERAL_H

#include <SDL2/SDL.h>
#include <printf.h>

#include "rac0_base.h"

typedef struct {
    rac0_value_t keys[322];
    rac0_u32_t* pixels;
    rac0_value_t resolution_width;
    rac0_value_t resolution_height;
}  sdl_peripheral_devices_data_t;

void sdl_peripheral_initialize(sdl_peripheral_devices_data_t* data, rac0_value_t screen_width, rac0_value_t screen_height);
void sdl_peripheral_free(sdl_peripheral_devices_data_t* data);
void* sdl_peripheral_run(void* data);

void sdl_peripheral_screen_device_push(void* device_data, rac0_u64_t adress, rac0_value_t value);
rac0_value_t sdl_peripheral_screen_device_pool(void* device_data, rac0_u64_t adress);

void sdl_peripheral_keyboard_device_push(void* device_data, rac0_u64_t adress, rac0_value_t value);
rac0_value_t sdl_peripheral_keyboard_device_pool(void* device_data, rac0_u64_t adress);

#endif