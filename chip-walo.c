#include "headers/system.h"
#include "headers/peripherals.h"
#include <unistd.h>
#include <stdio.h>

extern Processor chip8;

int main(int argc, char *argv[])
{
    // Load Rom
    if(argc > 1) 
    {
        if(argc == 2) { romLoad(argv[1]); }
        else { printf("please input single rom only."); return 1; }
    }
    else { printf("Input Rom, Try again."); return 1; }

    // Initialization
    initSys();
    initPeripherals();
    initSound();

    // Game loop
    while(1)
    {
        cpuCycle();
        keyPress();
        if(chip8.drawFlag == 1) { drawGraphics(); chip8.drawFlag = 0; }
        if(chip8.soundFlag == 1) { beepSound(); chip8.soundFlag = 0; }
        if(exitMachine()) { break; }
        usleep(2000); // Chip8 run incredibly fast without this (SPEED HACK)
    }
    
    // Deinitialization
    closePeripherals();
    closeAudio();

    return 0;
}