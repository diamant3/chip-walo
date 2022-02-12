#ifndef SYSTEM_H
    #define SYSTEM_H

    // use c99 data types
    #include <stdint.h>

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
    void rom_load(const char *rom);
    void cpu_cycle(); 
    void init_sys();

    typedef struct chip8 {
        uint8_t V[16];
        uint16_t I;
        uint16_t pc;
        uint16_t opcode;
        uint8_t memory[4096];
        uint16_t stack[16];
        uint16_t sp;
        uint8_t gfx[64 * 32];
        uint8_t delayTimer;
        uint8_t soundTimer;
        uint8_t keypad[16];
        uint8_t drawFlag;
        uint8_t soundFlag;
    } Processor;

    extern Processor chip8;

    // Response
    extern uint8_t quit;


#endif