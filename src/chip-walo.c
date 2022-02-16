#include "system.h"
#include "peripherals.h"
#include <stdio.h>

int32_t main (int32_t argc, const int8_t *argv[]) {
    // Load Rom
    if (argc != 2) {
        printf("Usage: chip-walo \"rom.ch8\"");
        return TRUE;
    } else {
        rom_load(argv[1]);
    }

    // Initialization
    init_sys();
    init_graphics();
    init_audio();

    // cpu ops loop
    while (TRUE) {
        key_press();
        cpu_cycle();

        if (chip8.drawFlag) { 
            draw_graphics(); 
            chip8.drawFlag = TRUE; 
        }

        if (chip8.soundFlag) { 
            beep_audio(); 
            chip8.soundFlag = FALSE; 
        }

        // Deinitialization
        if (quit) {
            close_audio();
            close_graphics();
            break;
        }

        SDL_Delay(4); // Limit speed
    }
    return 0;
}