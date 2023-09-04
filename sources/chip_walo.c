#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "chip_walo.h"
#include "opcodes.h"

static const unsigned char FONTSET[FONT_SZ] = 
{
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

Chip_walo *chip_walo = NULL;

void CW_init(void) 
{
    chip_walo = calloc(13, sizeof(*chip_walo));
    if (chip_walo == NULL) {
        return;
    }

    srand((unsigned int)time(NULL));
    for (unsigned char font = 0; font < FONT_SZ; font++) { chip_walo->mem[font] = FONTSET[font]; }
    chip_walo->pc = MEM_START_ADDR;
}

void CW_deinit(void) 
{
    free(chip_walo);
    chip_walo = NULL;
}

int CW_loadrom(const char *rom) 
{
    size_t rom_sz = 0;
    unsigned char *rom_buf = NULL;
    FILE *file = NULL;

    file = fopen(rom, "rb");
    if (file == NULL) { return -1; }
    if (fseek(file, 0, SEEK_END) != 0) 
    { 
        fclose(file); 
        return -1; 
    }

    rom_sz = ftell(file);
    if (rom_sz == -1) 
    { 
        fclose(file); 
        return -1; 
    }

    if (rom_sz >= (MEM_SZ - MEM_START_ADDR)) 
    { 
        fclose(file); 
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) 
    { 
        fclose(file); 
        return -1; 
    }

    rom_buf = calloc(rom_sz, sizeof(*rom_buf));
    if (rom_buf == NULL) 
    { 
        fclose(file); 
        return -1; 
    }

    if (fread(rom_buf, sizeof(*rom_buf), rom_sz, file) != rom_sz) 
    { 
        fclose(file); 
        free(rom_buf); 
        return -1; 
    }

    for (size_t data = 0; data < rom_sz; data++) { chip_walo->mem[MEM_START_ADDR + data] = rom_buf[data]; }

    free(rom_buf);
    fclose(file);
    return 0;
}

void CW_cycle(void) 
{
    chip_walo->opcode = (chip_walo->mem[chip_walo->pc] << 8) | chip_walo->mem[chip_walo->pc + 1];
    printf("[STATE][OPCODE] 0x%x\n", (chip_walo->opcode));
    oc_exec();

    if (chip_walo->dt > 0) { chip_walo->dt--; }
    if (chip_walo->st > 0) 
    {
        chip_walo->st--;
        chip_walo->audio_flag = 1;
    }
}
