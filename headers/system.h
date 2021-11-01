#ifndef SYSTEM_H
#define SYSTEM_H

// Instructions
#define NNN   (chip8.opcode & 0x0FFF)
#define byte  (chip8.opcode & 0x00FF) 
#define N     (chip8.opcode & 0x000F)
#define vx    (chip8.opcode & 0x0F00) >> 8
#define vy    (chip8.opcode & 0x00F0) >> 4

// Sizes
#define START_ADDRESS 0x200
#define FONT_SET      80
#define GFX_SIZE      (64 * 32)
#define KEY_LENGTH    16

// Functions
void romLoad(const char *rom);
void cpuCycle(); 
void initSys();

typedef struct chip8 {
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned short stack[16];
    unsigned short sp;
    unsigned char gfx[64 * 32];
    unsigned char delayTimer;
    unsigned char soundTimer;
    unsigned char keypad[16];
    unsigned char drawFlag;
    unsigned char soundFlag;
} Processor; 

#endif