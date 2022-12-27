#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <stdint.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

/* A 12-bit value, the lowest 12 bits of the instruction */
#define ADDR (cw->opcode & 0x0FFF)
/* An 8-bit value, the lowest 8 bits of the instruction */
#define BYTE (cw->opcode & 0x00FF)
/* A 4-bit value, the lowest 4 bits of the instruction */
#define NIBBLE (cw->opcode & 0x000F)
/* A 4-bit value, the lower 4 bits of the high byte of the instruction */
#define X ((cw->opcode & 0x0F00) >> 8)
/* A 4-bit value, the upper 4 bits of the low byte of the instruction */
#define Y ((cw->opcode & 0x00F0) >> 4)

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
    u8 reg_v[REGISTER_COUNT];
    u8 mem[MEMORY_SIZE];
    u8 gfx[SCREEN_SIZE];
    u8 key[KEY_COUNT];
    u8 dt;
    u8 st;
    u8 draw_flag;
    u8 audio_flag;
    u16 i;
    u16 pc;
    u16 opcode;
    u16 stack[STACK_SIZE];
    u16 sp;
} Chip_walo;
Chip_walo *chip_walo;

void core_load(Chip_walo *cw, const char *rom);
void core_cycle(Chip_walo *cw);
void core_init(Chip_walo *cw);
#endif