#ifndef CHIP_WALO_H
#define CHIP_WALO_H

// Address Locations
#define ADDR (chip_walo->opcode & 0x0fff)
#define BYTE (chip_walo->opcode & 0x00ff)
#define NIBBLE (chip_walo->opcode & 0x000f)
#define X ((chip_walo->opcode & 0x0f00) >> 8)
#define Y ((chip_walo->opcode & 0x00f0) >> 4)

// Chip-8 System Specs
#define SCR_W           64
#define SCR_H           32
#define SCR_SZ          2048
#define FONT_SZ         80
#define STACK_SZ        16
#define MEM_SZ          4096
#define MEM_START_ADDR  512
#define REG_SZ          16
#define KEY_SZ          16
#define FONT_START_ADDR 80

typedef struct Chip_walo_t {
    unsigned char reg_v[REG_SZ];
    unsigned char mem[MEM_SZ];
    unsigned char gfx[SCR_SZ];
    unsigned char key[KEY_SZ];
    unsigned char dt;
    unsigned char st;
    unsigned char draw_flag;
    unsigned char audio_flag;
    unsigned short i;
    unsigned short pc;
    unsigned short opcode;
    unsigned short stack[STACK_SZ];
    unsigned short sp;
} Chip_walo;
extern Chip_walo *chip_walo;

int CW_loadrom(const char *rom);
void CW_init(void);
void CW_deinit(void);
void CW_cycle(void);

#endif
