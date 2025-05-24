#include "sdl_peripheral.h" 

void sdl_peripheral_initialize(sdl_peripheral_devices_data_t* data, rac0_value_t screen_width, rac0_value_t screen_height) {
    for(int i = 0; i < 322; i++)
        data->keys[i] = 0;
    
    data->pixels = calloc(sizeof(rac0_u32_t) * screen_width * screen_width, 1);
    data->resolution_width = screen_width;
    data->resolution_height = screen_height;
}

void sdl_peripheral_free(sdl_peripheral_devices_data_t* data) {
    free(data->pixels);
}

void* sdl_peripheral_run(void* data) {
    sdl_peripheral_devices_data_t* sdl_peripheral_data = (sdl_peripheral_devices_data_t*) data; 

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize the SDL2 library\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Window *window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 720, 0);
    SDL_SetWindowResizable(window, SDL_FALSE);

    if(!window) {
        printf("Failed to create window\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_SOFTWARE);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, sdl_peripheral_data->resolution_width, sdl_peripheral_data->resolution_height);

    char keep_window_open = 1;
    while(keep_window_open) {
        SDL_Event e;

        while(SDL_PollEvent(&e) > 0) {
            int code = e.key.keysym.sym;

            switch(e.type) {
                case SDL_QUIT:
                    keep_window_open = 0;
                    break;
                case SDL_KEYDOWN:
                    if(code >= 0 && code < 322)
                        sdl_peripheral_data->keys[code] = 1;
                    break;
                case SDL_KEYUP:
                    if(code >= 0 && code < 322)
                        sdl_peripheral_data->keys[code] = 0;
                    break;
                default:
                    break;
            }

            SDL_UpdateWindowSurface(window);
        }
        
        int pitch, w, h;
        void* pixels;

        SDL_LockTexture(texture, NULL, &pixels, &pitch);
        unsigned int* upixels = (unsigned int*) pixels;
        
        for(int i = 0; i < sdl_peripheral_data->resolution_height * sdl_peripheral_data->resolution_width; ++i)
            upixels[i] =  sdl_peripheral_data->pixels[i];
 
        SDL_UnlockTexture(texture);

        SDL_Rect texture_rect;
        texture_rect.x = 0;
        texture_rect.y = 0;
        texture_rect.w = 800;
        texture_rect.h = 720;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, &texture_rect); 
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return NULL;
}

void sdl_peripheral_screen_device_push(void* device_data, rac0_u64_t address, rac0_value_t value) {
    sdl_peripheral_devices_data_t* sdl_peripheral_data = (sdl_peripheral_devices_data_t*) device_data; 

    if(address >= 0 && address < sdl_peripheral_data->resolution_height * sdl_peripheral_data->resolution_width)
        sdl_peripheral_data->pixels[address] = value;
}

rac0_value_t sdl_peripheral_screen_device_pool(void* device_data, rac0_u64_t address) {
    sdl_peripheral_devices_data_t* sdl_peripheral_data = (sdl_peripheral_devices_data_t*) device_data; 

    return 0;
}

void sdl_peripheral_keyboard_device_push(void* device_data, rac0_u64_t address, rac0_value_t value) {
    sdl_peripheral_devices_data_t* sdl_peripheral_data = (sdl_peripheral_devices_data_t*) device_data; 

}

rac0_value_t sdl_peripheral_keyboard_device_pool(void* device_data, rac0_u64_t address) {
    sdl_peripheral_devices_data_t* sdl_peripheral_data = (sdl_peripheral_devices_data_t*) device_data; 

    if(address >= 0 && address < 322)
        return sdl_peripheral_data->keys[address];

    return 0;
}
