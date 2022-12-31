#ifndef _SYSTEM_H
#define _SYSTEM_H

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
    unsigned char reg_v[REGISTER_COUNT];
    unsigned char mem[MEMORY_SIZE];
    unsigned char gfx[SCREEN_SIZE];
    unsigned char key[KEY_COUNT];
    unsigned char dt;
    unsigned char st;
    unsigned char draw_flag;
    unsigned char audio_flag;
    unsigned short i;
    unsigned short pc;
    unsigned short opcode;
    unsigned short stack[STACK_SIZE];
    unsigned short sp;
} Chip_walo;
Chip_walo *chip_walo;

void core_load(const char *rom);
void core_cycle(void);
void core_init(void);
void core_deinit(void);

char running;
#endif