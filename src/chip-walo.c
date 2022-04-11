#include <stdio.h>
#include "system.h"
#include "peripherals.h"

int main(int argc, char *argv[]) {
    // Load Rom
    if (argc != 2) {
        printf("Usage: chip-walo \"rom.ch8\"");
        return 1;
    } 
    else {
        load_rom(argv[1]);
        init_sys();
        create_peripherals();
    }
    
    // cpu ops loop
    while (1) {
        detect_key();
        cpu_cycle();

        if (chip8.drawFlag == 1) { 
            draw_pixel(); 
            chip8.drawFlag = 0; 
        }

        if (chip8.soundFlag == 1) { 
            beep(); 
            chip8.soundFlag = 0; 
        }

        if (quit == 1) {
            destroy_peripherals();
            break;
        }
    }
    return 0;
}