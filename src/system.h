#ifndef _SYSTEM_H
#define _SYSTEM_H

// opcode shorthands
#define ADDR   (cw->opcode & 0x0FFF) /* A 12-bit value, the lowest 12 bits of the instruction */
#define BYTE   (cw->opcode & 0x00FF) /* An 8-bit value, the lowest 8 bits of the instruction */
#define NIBBLE (cw->opcode & 0x000F) /* A 4-bit value, the lowest 4 bits of the instruction */
#define X      ((cw->opcode & 0x0F00) >> 8) /* A 4-bit value, the lower 4 bits of the high byte of the instruction */
#define Y      ((cw->opcode & 0x00F0) >> 4) /* A 4-bit value, the upper 4 bits of the low byte of the instruction */

// system constants
#define SCREEN_WIDTH         64    /* width of the display */
#define SCREEN_HEIGHT        32    /* height of the display */
#define SCREEN_SIZE          2048  /* size of displayed graphics in pixel (64 x 32) */
#define FONT_LENGTH          80    /* length of built-in font set */
#define STACK_SIZE           16    /* size of stack register */
#define MEMORY_SIZE          4096  /* size of the memory */
#define MEMORY_ADDRESS_START 512   /* start address of common chip8 rom load */
#define REGISTER_COUNT       16

// chip-walo system struct
typedef struct Chip_walo_t {
    uint8_t reg_v[REGISTER_COUNT];
    uint8_t mem[MEMORY_SIZE];
    uint8_t gfx[SCREEN_SIZE];
    uint8_t keypad[16];
    uint8_t DT;
    uint8_t ST;
    uint8_t draw_flag;
    uint8_t audio_flag;
    uint16_t I;
    uint16_t PC;
    uint16_t opcode;
    uint16_t stack[STACK_SIZE];
    uint16_t SP;
} Chip_walo;

uint8_t quit;

// system/cpu functions
void core_load(Chip_walo *cw, const uint8_t *rom);
void core_cycle(Chip_walo *cw);
void core_init(Chip_walo *cw);
#endif