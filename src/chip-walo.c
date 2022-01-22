#include "../header/system.h"
#include "../header/peripherals.h"
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
    init_peripherals();
    init_sound();

    // cpu ops loop
    while(1)
    {
        SDL_Delay(8); // Limit speed
        cpu_cycle();
        key_press();

        if(chip8.drawFlag == 1) { 
            draw_graphics(); 
            chip8.drawFlag = 0; 
        }

        if(chip8.soundFlag == 1) { 
            beep_sound(); 
            chip8.soundFlag = 0; 
        }

        if(quit == 1) {
            // Deinitialization
            close_audio();
            close_peripherals();
            break;
        }
    }

    return 0;
}