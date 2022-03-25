#include <stdio.h>
#include "system.h"
#include "peripherals.h"

int main(int argc, char* argv[]) {
    // Load Rom
    if (argc == 2) {
        load_rom(argv[1]);
    } else {
        printf("Usage: chip-walo \"rom.ch8\"");
        return 1;
    }

    // Initialization
    init_sys();
    init_graphics();
    init_audio();

    // cpu ops loop
    while (1) {
        key_press();
        cpu_cycle();

        if (chip8.drawFlag) { 
            draw_graphics(); 
            chip8.drawFlag = 0; 
        }

        if (chip8.soundFlag) { 
            beep_audio(); 
            chip8.soundFlag = 0; 
        }

        // Deinitialization
        if (quit) {
            close_audio();
            close_graphics();
            break;
        }
    }
    return 0;
}