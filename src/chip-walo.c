#include "system.h"
#include "peripherals.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    // Load Rom
    if(argc != 2) 
    {
        printf("Usage: chip-walo \"rom.ch8\"");
        return 1;
    } else {
        rom_load(argv[1]);
    }

    // Initialization
    init_sys();
    init_graphics();
    init_audio();

    // cpu ops loop
    while(1)
    {
        key_press();
        cpu_cycle();

        if(chip8.drawFlag == 1) { 
            draw_graphics(); 
            chip8.drawFlag = 0; 
        }

        if(chip8.soundFlag == 1) { 
            beep_audio(); 
            chip8.soundFlag = 0; 
        }

        // Deinitialization
        if(quit == 1) {
            close_audio();
            close_graphics();
            break;
        }

        SDL_Delay(4); // Limit speed
    }

    return 0;
}