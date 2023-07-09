#include <stdlib.h>
#include <stdio.h>

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

void (*oc_handlers[0x10000])(void);
Chip_walo *chip_walo = NULL;

void CW_init(void) 
{
    chip_walo = calloc(13, sizeof(Chip_walo));
    if (chip_walo == NULL) {
        return;
    }

    for (int font = 0; font < FONT_SZ; font++) { chip_walo->mem[FONT_START_ADDR + font] = FONTSET[font]; }
    chip_walo->pc = MEM_START_ADDR;

    // Set array of all function pointers to null(it means unknown opcode)
    for (int a = 0x0000; a <= 0x10000; a++) { oc_handlers[a] = &oc_null; }

    // opcode 0
    oc_handlers[0x00e0] = &oc_00e0;
    oc_handlers[0x00ee] = &oc_00ee;

    // opcode 1
    for (int a = 0x1000; a <= 0x1fff; a++) { oc_handlers[a] = &oc_1nnn; }

    // opcode 2
    for (int a = 0x2000; a <= 0x2fff; a++) { oc_handlers[a] = &oc_2nnn; }

    // opcode 3
    for (int a = 0x3000; a <= 0x3fff; a++) { oc_handlers[a] = &oc_3xnn; }

    // opcode 4
    for (int a = 0x4000; a <= 0x4fff; a++) { oc_handlers[a] = &oc_4xnn; }

    // opcode 5
    for (int a = 0x5000; a <= 0x5ff0; a++) { oc_handlers[a] = &oc_5xy0; }

    // opcode 6
    for (int a = 0x6000; a <= 0x6fff; a++) { oc_handlers[a] = &oc_6xnn; }

    // opcode 7
    for (int a = 0x7000; a <= 0x7fff; a++) { oc_handlers[a] = &oc_7xnn; }

    // opcode 8
    for (int a = 0x8000; a <= 0x8ffe; a++) { oc_handlers[a] = &oc_8xyz; }

    // opcode 9
    for (int a = 0x9000; a <= 0x9ff0; a++) { oc_handlers[a] = &oc_9xy0; }

    // opcode A
    for (int a = 0xa000; a <= 0xafff; a++) { oc_handlers[a] = &oc_annn; }

    // opcode B
    for (int a = 0xb000; a <= 0xbfff; a++) { oc_handlers[a] = &oc_bxnn; }

    // opcode C
    for (int a = 0xc000; a <= 0xcfff; a++) { oc_handlers[a] = &oc_cxnn; }

    // opcode D
    for (int a = 0xd000; a <= 0xdfff; a++) { oc_handlers[a] = &oc_dxyn; }

    // opcode E
    for (int a = 0xe09e; a <= 0xef9e; a++) { oc_handlers[a] = &oc_ex9e; }
    for (int a = 0xef9f; a <= 0xefa1; a++) { oc_handlers[a] = &oc_exa1; }

    // opcode F
    for (int a = 0xf007; a <= 0xff65; a++) { oc_handlers[a] = &oc_fxzz; }    
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
    if (rom_sz == -1llu) 
    { 
        fclose(file); 
        return -1; 
    }

    if (rom_sz > (MEM_SZ - FONT_START_ADDR)) 
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
    // printf("[STATE][OPCODE] 0x%x\n", chip_walo->opcode);
    oc_handlers[chip_walo->opcode]();

    if (chip_walo->dt > 0) { chip_walo->dt--; }
    if (chip_walo->st > 0) 
    {
        chip_walo->st--;
        chip_walo->audio_flag = 1;
    }
}

/*
int main(int argc, char *argv[]) {
    CW_init();
    if (argc != 2) { return -1; }
    CW_loadrom(argv[1]);
    while (1) {
        CW_cycle();
        printf("opcode: %x \n", chip_walo->opcode);
    }

    CW_deinit();
    return 0;
}*/
