#include <stdio.h>

#include "common.h"

int32_t main(int32_t argc, int8_t *argv[]) {
    // Load Rom
    if (argc == 2) 
    {
        cw_system_cpu_file_load(argv[1]); 
        cw_system_cpu_create();
        cw_peripheral_graphic_create();
        cw_peripheral_audio_create();
    } 
    else 
    {
        printf("Usage: chip-walo \"rom.ch8\"\n");
        return 0;
    }

    // cpu operation loop
    while (true) {
        cw_system_cpu_cycle();
        cw_peripheral_key_detect();

        if (chip8.register_draw_flag == true) { 
            cw_peripheral_graphic_draw_pixel(); 
            chip8.register_draw_flag = false; 
        }

        if (chip8.register_sound_flag == true) { 
            cw_peripheral_audio_beep(); 
            chip8.register_sound_flag = false; 
        }

        if (quit == true) {
            cw_system_cpu_destroy();
            cw_peripheral_audio_destroy();
            cw_peripheral_graphic_destroy();
            break;
        }
    }
    return 0;
}