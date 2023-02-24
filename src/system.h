#ifndef _SYSTEM_H
#define _SYSTEM_H

#include<stdint.h>

/* A 12-bit value, the lowest 12 bits of the instruction */
#define ADDR (chip_walo->opcode & 0x0FFF)

/* An 8-bit value, the lowest 8 bits of the instruction */
#define BYTE (chip_walo->opcode & 0x00FF)

/* A 4-bit value, the lowest 4 bits of the instruction */
#define NIBBLE (chip_walo->opcode & 0x000F)

/* A 4-bit value, the lower 4 bits of the high byte of the instruction */
#define X ((chip_walo->opcode & 0x0F00) >> 8)

/* A 4-bit value, the upper 4 bits of the low byte of the instruction */
#define Y ((chip_walo->opcode & 0x00F0) >> 4)

/* width of the display */
#define SCREEN_WIDTH 64

/* height of the display */
#define SCREEN_HEIGHT 32

/* size of displayed graphics in pixel (64 x 32) */
#define SCREEN_SIZE 2048

/* length of built-in font set */
#define FONT_LENGTH 80

/* size of stack register */
#define STACK_SIZE 16

/* size of the memory */
#define MEMORY_SIZE 4096

/* start address of common chip8 rom load */
#define START_MEM_ADDRESS 0x200

/* register count */
#define REGISTER_COUNT 16

/* key count */
#define KEY_COUNT 16

/* font location in memory */
#define START_FONT_ADDRESS 0x50

// chip-walo system struct
typedef struct Chip_walo_t {
    uint8_t reg_v[REGISTER_COUNT];
    uint8_t mem[MEMORY_SIZE];
    uint8_t gfx[SCREEN_SIZE];
    uint8_t key[KEY_COUNT];
    uint8_t dt;
    uint8_t st;
    uint8_t draw_flag;
    uint8_t audio_flag;
    uint16_t i;
    uint16_t pc;
    uint16_t opcode;
    uint16_t stack[STACK_SIZE];
    uint16_t sp;
} Chip_walo;
extern Chip_walo *chip_walo;

void core_load(const char *rom);
void core_cycle(void);
void core_init(void);
void core_deinit(void);

extern uint8_t running;
#endif