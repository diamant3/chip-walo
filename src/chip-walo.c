#include <stdio.h>
#include "system.h"
#include "peripherals.h"

int8_t main(int8_t argc, char* argv[]) {
    // Load Rom
    if (argc == 2) {
        load_rom(argv[1]);
    } else {
        printf("Usage: chip-walo \"rom.ch8\"");
        return TRUE;
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
            chip8.drawFlag = FALSE; 
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

        SDL_Delay(4);
    }
    return 0;
}