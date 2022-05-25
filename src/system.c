#define __STDC_WANT_LIB_EXT1__ 1

#include "system.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

Processor_t chip8;
unsigned char fontSet[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/**
    @brief     Initialize the chip8 system
    @return    Nothing
*/
void init_sys(void) {
    srand((unsigned int)time(NULL));
    chip8.I = 0;
    chip8.pc = START_ADDRESS;
    chip8.opcode = 0;
    chip8.sp = 0;
    
    for (int font = 0; font < FONT_SET; ++font) {
        chip8.memory[font] = fontSet[font];
    }
}

/**
    @brief     Load a rom file ex: (.ch8 file)
    @param rom Path of the rom or the filename of rom.
    @return    Nothing
*/
void load_rom(const char *rom) {
    long romSize;
    char *romBuffer = NULL;
    FILE *file = NULL;

    if ((file = fopen(rom, "r")) != NULL) {
        // Get the rom size & buffer
        fseek(file, 0, SEEK_END);
        romSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        romBuffer = (char*) malloc(sizeof(char) * romSize);
    } else {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    }

    // Check and Load the rom to memory
    if (fread(romBuffer, 1, romSize, file) > 0) {
        for (long data = 0; data < romSize; ++data) {
            chip8.memory[START_ADDRESS + data] = romBuffer[data];
        }
    } else {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    }
    free(romBuffer);
    fclose(file);
}

/**
    @brief     Main instruction cycle (Fetch/Decode/Execute)
    @return    Nothing
*/
void cpu_cycle(void) {
    chip8.opcode = chip8.memory[chip8.pc] << 8 | chip8.memory[chip8.pc + 1]; 

    switch (chip8.opcode & 0xF000) {
        case 0x0000:
            switch (byte) {
                // Clear the display
                case 0xE0:
                    memset(chip8.gfx, 0, GFX_SIZE);
                    chip8.pc += 2;
                    break;

                // Return from a subroutine
                case 0xEE:
                    --chip8.sp;
                    chip8.pc = chip8.stack[chip8.sp];
                    chip8.pc += 2;
                    break;
                default:
                    printf("Unknown op code: %x\n", chip8.opcode);
                    break;
            }
            break;

        // Jump to location NNN
        case 0x1000:
            chip8.pc = NNN;
            break;

        // Call subroutine at NNN
        case 0x2000:
            chip8.stack[chip8.sp] = chip8.pc;
            ++chip8.sp;
            chip8.pc = NNN;
            break;

        // Skip next instruction if register Vx == byte
        case 0x3000:
            if (chip8.V[vx] == byte) { 
                chip8.pc += 4;
            } else {
                chip8.pc += 2;
            }
            break;

        // Skip next instruction if register Vx != byte
        case 0x4000:
            if (chip8.V[vx] != byte) {
                chip8.pc += 4; 
            } else {
                chip8.pc += 2;
            }
            break;

        // Skip next instruction if register Vx == register Vy
        case 0x5000:
            if (chip8.V[vx] == chip8.V[vy]) {
                chip8.pc += 4; 
            } else {
                chip8.pc += 2;
            }
            break;

        // Set register Vx = byte
        case 0x6000:
            chip8.V[vx] = byte;
            chip8.pc += 2;
            break;

       // Set register Vx = Vx + byte
        case 0x7000:
            chip8.V[vx] += byte;
            chip8.pc += 2;
            break;
        case 0x8000:
            switch (N) {
                // Set register Vx = Vy
                case 0x0:
                    chip8.V[vx] = chip8.V[vy];
                    chip8.pc += 2;
                    break;

                // Set register Vx = Vx OR Vy
                case 0x1:
                    chip8.V[vx] |= chip8.V[vy];
                    chip8.pc += 2;
                    break;

                // Set register Vx = Vx AND Vy
                case 0x2:
                    chip8.V[vx] &= chip8.V[vy];
                    chip8.pc += 2;
                    break;

                // Set register Vx = Vx XOR Vy
                case 0x3:
                    chip8.V[vx] ^= chip8.V[vy];
                    chip8.pc += 2;
                    break;

                // Set register Vx = Vx + Vy, Set register Vf = 1 if overflow
                case 0x4:
                    {
                        chip8.V[0xF] = 0;
                        int result = chip8.V[vx] + chip8.V[vy];
                        unsigned char overflow = 0;

                        if (result > 255) {
                            overflow = 1;
                        } else {
                            overflow = 0;
                        }
                        chip8.V[vx] = result & 255;
                        chip8.V[0xF] = overflow;
                        chip8.pc += 2;
                    }
                    break;

                // Set register Vx = Vx - Vy, Set register Vf = NOT borrow
                case 0x5:
                    {
                        chip8.V[0xF] = 0;
                        unsigned char overflow = 0;

                        if (chip8.V[vx] >= chip8.V[vy]) {
                            overflow = 1;
                        } else {
                            overflow = 0;
                        }
		    	        chip8.V[vx] = chip8.V[vx] - chip8.V[vy];
			            chip8.V[0xF] = overflow;                    
                        chip8.pc += 2;
                    }
                    break;

                // Set register Vx = Vx SHR 1
                case 0x6:
                    {
                        unsigned char overflow = chip8.V[vx] & 1;
                        chip8.V[vx] >>= 1;
                        chip8.V[0xF] = overflow;
                        chip8.pc += 2;
                    }
                    break;

                // Set register Vx = Vy - Vx, Set register Vf = NOT borrow
                case 0x7:
                    {
                        chip8.V[0xF] = 0;
                        unsigned char overflow = 0;

                        if (chip8.V[vy] >= chip8.V[vx]) {
                            overflow = 1;
                        } else {
                            overflow = 0;
                        }
			            chip8.V[vx] = chip8.V[vy] - chip8.V[vx];
			            chip8.V[0xF] = overflow;
                        chip8.pc += 2;
                    }
                    break;

                // Set register Vx = Vx SHL 1
                case 0xE:       
                {
                    unsigned char overflow = 0;
                    
                    if (chip8.V[vx] >> 7) {
                        overflow = 1;
                    } else {
                        overflow = 0;
                    }
                    chip8.V[vx] <<= 1;
                    chip8.V[0xF] = overflow;
                    chip8.pc += 2;
                }
                break;

                default:
                    printf("Unknown op code: %x\n", chip8.opcode);
                    break;
            }
            break;

        // Skip next instruction if register Vx != Vy
        case 0x9000:
            if (chip8.V[vx] != chip8.V[vy]) { 
                chip8.pc += 4; 
            } else {
                chip8.pc += 2;
            }
            break;

        // Set I = NNN
        case 0xA000:
            chip8.I = NNN;
            chip8.pc += 2;
            break;

        // Jump to location NNN + register V0
        case 0xB000:
            chip8.pc = chip8.V[0] + NNN;
            break;

        // Set register Vx = random byte AND byte
        case 0xC000:
            chip8.V[vx] = (rand() % 256) & byte;
            chip8.pc += 2;
            break;

        // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
        case 0xD000:		   
            {
                unsigned short x = chip8.V[vx];
                unsigned short y = chip8.V[vy];
                unsigned short height = N;
                unsigned short pixel;
                chip8.V[0xF] = 0;

                for (int yline = 0; yline < height; ++yline) {
                    pixel = chip8.memory[chip8.I + yline];
                    for (int xline = 0; xline < 8; ++xline) {
                        if ((pixel & (128 >> xline)) != 0) {
                            if (chip8.gfx[(x + xline + ((y + yline) * 64))] == 1) {
                                chip8.V[0xF] = 1;
                            }                                 
                            chip8.gfx[(x + xline + ((y + yline) * 64)) % (GFX_SIZE)] ^= 1;
                        }
                    }
                }
                chip8.drawFlag = 1;
                chip8.pc += 2;
            }
            break;
        
        case 0xE000:
            switch(byte)
            {
                // Skip next instruction if key with the value of register Vx is pressed
                case 0x9E:
                    if (chip8.keypad[chip8.V[vx]] == 1) { 
                        chip8.pc += 4; 
                    } else {
                        chip8.pc += 2;
                    }
                    break;

                // Skip next instruction if key with the value of Vx is not pressed
                case 0xA1:
                    if (chip8.keypad[chip8.V[vx]] == 0) {
                        chip8.pc += 4; 
                    } else {
                        chip8.pc += 2;
                    }
                    break;

                default:
                    printf("Unknown op code: %x\n", chip8.opcode);
                    break;
            }
            break;
        case 0xF000:
            switch (byte)
            {
                // Set register Vx = delay timer value
                case 0x07:
                    chip8.V[vx] = chip8.delayTimer;
                    chip8.pc += 2;
                    break;

                // Wait for a key press, store the value of the key in register Vx
                case 0x0A:
                    for (int key = 0; key < KEY_LENGTH; ++key) {
                        if(chip8.keypad[key] == 1) {
                            chip8.V[vx] = key;
                            chip8.pc += 2;
                            break;
                        }
                    }
                    break;

                // Set delay timer = register Vx
                case 0x15:
                    chip8.delayTimer = chip8.V[vx];
                    chip8.pc += 2;
                    break;
                
                // Set sound timer = Vx
                case 0x18:
                    chip8.soundTimer = chip8.V[vx];
                    chip8.pc += 2;
                    break;

                // Set I = I + register Vx
                case 0x1E:
                    chip8.I += chip8.V[vx];
                    chip8.pc += 2;
                    break;

                // Set I = location of sprite for digit register Vx
                case 0x29:
                    chip8.I = (chip8.V[vx] * 5);
                    chip8.pc += 2;
                    break;

                // Store BCD representation of register Vx in memory locations I, I+1, and I+2
                case 0x33:
                    chip8.memory[chip8.I] = (chip8.V[vx] / 100) % 10;
                    chip8.memory[chip8.I + 1] = (chip8.V[vx] / 10) % 10;
                    chip8.memory[chip8.I + 2] = chip8.V[vx] % 10;
                    chip8.pc += 2;
                    break;
                
                // Store registers V0 through Vx in memory starting at location I
                case 0x55:
                    for (int vi = 0; vi <= vx; ++vi) {
                        chip8.memory[chip8.I + vi] = chip8.V[vi];
                    }
                    chip8.pc += 2;
                    break;

                // Read registers V0 through Vx from memory starting at location I
                case 0x65:
                    for (int vi = 0; vi <= vx; ++vi) {
                        chip8.V[vi] = chip8.memory[chip8.I + vi];
                    }
                    chip8.pc += 2;
                    break;

                default:
                    printf("Unknown op code: %x\n", chip8.opcode);
                    break;
            }
        break;
    }  

    // Update Timers
    if (chip8.delayTimer > 0) { 
        --chip8.delayTimer; 
    }

    if (chip8.soundTimer > 0) {
        --chip8.soundTimer;
        chip8.soundFlag = 1;
    }
}