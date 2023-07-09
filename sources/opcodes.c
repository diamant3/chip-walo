#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chip_walo.h"
#include "opcodes.h"

void oc_null(void) 
{
    printf("[STATE][OPCODE] Unknown :(\n");
}

void oc_00e0(void)
{
    chip_walo->draw_flag = 1;
    chip_walo->pc += 2;
}

void oc_00ee(void)
{
    chip_walo->pc = chip_walo->stack[chip_walo->sp];
    chip_walo->stack[chip_walo->sp] = 0;
    chip_walo->sp--;
    chip_walo->pc += 2;
}

void oc_1nnn(void)
{
    chip_walo->pc = ADDR;
}

void oc_2nnn(void) 
{
    chip_walo->sp++;
    chip_walo->stack[chip_walo->sp] = chip_walo->pc;
    chip_walo->pc = ADDR;
}

void oc_3xnn(void) 
{
    if (chip_walo->reg_v[X] == BYTE) { chip_walo->pc += 4; }
    else { chip_walo->pc += 2; }
}

void oc_4xnn(void) 
{
    if (chip_walo->reg_v[X] != BYTE) { chip_walo->pc += 4; }
    else { chip_walo->pc += 2; }
}

void oc_5xy0(void) 
{
    if (chip_walo->reg_v[X] == chip_walo->reg_v[Y]) { chip_walo->pc += 4; }
    else { chip_walo->pc += 2; }
}

void oc_6xnn(void) 
{
    chip_walo->reg_v[X] = BYTE;
    chip_walo->pc += 2;
}

void oc_7xnn(void) 
{
    chip_walo->reg_v[X] += BYTE;
    chip_walo->pc += 2;
}

static void oc_8xy0(void)
{
    chip_walo->reg_v[X] = chip_walo->reg_v[Y];
    chip_walo->pc += 2;
}

static void oc_8xy1(void)
{
    chip_walo->reg_v[X] |= chip_walo->reg_v[Y];
    chip_walo->reg_v[0xf] = 0;
    chip_walo->pc += 2;
}

static void oc_8xy2(void)
{
    chip_walo->reg_v[X] &= chip_walo->reg_v[Y];
    chip_walo->reg_v[0xf] = 0;
    chip_walo->pc += 2;    
}

static void oc_8xy3(void)
{
    chip_walo->reg_v[X] ^= chip_walo->reg_v[Y];
    chip_walo->reg_v[0xf] = 0;
    chip_walo->pc += 2;    
}

static void oc_8xy4(void)
{
    unsigned short result = chip_walo->reg_v[X] + chip_walo->reg_v[Y];
    if (result > 0xff) { chip_walo->reg_v[0xf] = 1; }
    else { chip_walo->reg_v[0xf] = 0; }

    chip_walo->reg_v[X] = (result & 0xff);
    chip_walo->pc += 2;     
}

static void oc_8xy5(void)
{
    unsigned char result = chip_walo->reg_v[X] - chip_walo->reg_v[Y];
    if (chip_walo->reg_v[X] > chip_walo->reg_v[Y]) { chip_walo->reg_v[0xf] = 1; }
    else { chip_walo->reg_v[0xf] = 0; }

    chip_walo->reg_v[X] = result;
    chip_walo->pc += 2;
}

static void oc_8xy6(void)
{
    unsigned char result = (chip_walo->reg_v[X] & 0x1);
    chip_walo->reg_v[X] >>= 1;
    chip_walo->reg_v[0xf] = result;
    chip_walo->pc += 2;    
}

static void oc_8xy7(void)
{
    unsigned char result = chip_walo->reg_v[Y] - chip_walo->reg_v[X];
    if (chip_walo->reg_v[Y] > chip_walo->reg_v[X]) { chip_walo->reg_v[0xf] = 1; }
    else { chip_walo->reg_v[0xf] = 0; }

    chip_walo->reg_v[X] = result;
    chip_walo->pc += 2;    
}

static void oc_8xye(void)
{
    unsigned char result = (chip_walo->reg_v[X] << 7) & 1;
    chip_walo->reg_v[X] <<= 1;
    chip_walo->reg_v[0xf] = result;
    chip_walo->pc += 2;    
}

// main function for opcode 8
void oc_8xyz(void)
{
    switch(NIBBLE)
    {
        case 0x0:
            oc_8xy0();
            break;
        case 0x1:
            oc_8xy1();
            break;
        case 0x2:
            oc_8xy2();
            break;
        case 0x3:
            oc_8xy3();
            break;
        case 0x4:
            oc_8xy4();
            break;
        case 0x5:
            oc_8xy5();
            break;
        case 0x6:
            oc_8xy6();
            break;
        case 0x7:
            oc_8xy7();
            break;
        case 0xe:
            oc_8xye();
            break;
    }
}

void oc_9xy0(void) 
{
    if (chip_walo->reg_v[X] != chip_walo->reg_v[Y]) { chip_walo->pc += 4; }
    else { chip_walo->pc += 2; }
}

void oc_annn(void)
{
    chip_walo->i = ADDR;
    chip_walo->pc += 2;
}

void oc_bxnn(void) 
{
    const unsigned char XNN = (unsigned char)(ADDR & 0x0F00);
    chip_walo->pc = (chip_walo->reg_v[XNN] + ADDR);
}

void oc_cxnn(void) 
{
    srand((unsigned int)time(NULL));
    const unsigned char random = (rand() % 0xff);
    chip_walo->reg_v[X] = (random & BYTE);
    chip_walo->pc += 2;
}

void oc_dxyn(void) 
{
    const unsigned char XX = chip_walo->reg_v[X] % SCR_W;
    const unsigned char YY = chip_walo->reg_v[Y] % SCR_H;
    const unsigned char H = NIBBLE;

    for (unsigned char yline = 0; yline < H; yline++)
    {
        if ((YY + yline) >= SCR_H) { break; }
        const unsigned char px = chip_walo->mem[chip_walo->i + yline];
        for (unsigned char xline = 0; xline < 8; xline++)
        {
            if ((XX + xline) >= SCR_W) { break; }

            if (px & (0x80 >> xline))
            {
                const int TARGET = XX + xline + (YY + yline) * SCR_W;
                if (chip_walo->gfx[TARGET])
                {
                    chip_walo->reg_v[0xf] = 1; // collision
                }

                chip_walo->gfx[TARGET] ^= 1;
            }
        }
    }

    chip_walo->draw_flag = 1;
    chip_walo->pc += 2;
}

void oc_ex9e(void)
{
    if (chip_walo->key[chip_walo->reg_v[X]] == 1) { chip_walo->pc += 4; }
    else { chip_walo->pc += 2; }
}

void oc_exa1(void)
{
    if (chip_walo->key[chip_walo->reg_v[X]] == 0) { chip_walo->pc += 4; }
    else { chip_walo->pc += 2; }
}

static void oc_fx07(void)
{
    chip_walo->reg_v[X] = chip_walo->dt;
    chip_walo->pc += 2;
}

static void oc_fx0a(void)
{
    for (unsigned char key = 0; key < 16; key++) 
    {
        if (chip_walo->key[key] == 1) {
            chip_walo->reg_v[X] = key;
            chip_walo->pc += 2;
            break;
        }
    }
}

static void oc_fx15(void)
{
    chip_walo->dt = chip_walo->reg_v[X];
    chip_walo->pc += 2;
}

static void oc_fx18(void)
{
    chip_walo->st = chip_walo->reg_v[X];
    chip_walo->pc += 2; 
}

static void oc_fx1e(void)
{
    chip_walo->i += chip_walo->reg_v[X];
    chip_walo->pc += 2;
}

static void oc_fx29(void)
{
    chip_walo->i = (chip_walo->reg_v[X] * 5);
    chip_walo->pc += 2;
}

static void oc_fx33(void)
{
    chip_walo->mem[chip_walo->i] = (chip_walo->reg_v[X] / 100);
    chip_walo->mem[chip_walo->i + 1] = (chip_walo->reg_v[X] / 10) % 10;
    chip_walo->mem[chip_walo->i + 2] = (chip_walo->reg_v[X] % 100) % 10;
    chip_walo->pc += 2;
}

static void oc_fx55(void)
{
    for (unsigned char v = 0; v < X; v++) 
    {
        chip_walo->i++;
        chip_walo->mem[chip_walo->i + v] = chip_walo->reg_v[v];
    }

    chip_walo->pc += 2;
}

static void oc_fx65(void)
{
    for (unsigned char v = 0; v < X; v++)
    {
        chip_walo->i++;
        chip_walo->reg_v[v] = chip_walo->mem[chip_walo->i + v];
    }

    chip_walo->pc += 2;
}

// main function of opcode F
void oc_fxzz(void)
{
    switch (BYTE)
    {
        case 0x7:
            oc_fx07();
            break;
        case 0xa:
            oc_fx0a();
            break;
        case 0x15:
            oc_fx15();
            break;
        case 0x18:
            oc_fx18();
            break;
        case 0x1e:
            oc_fx1e();
            break;
        case 0x29:
            oc_fx29();
            break;
        case 0x33:
            oc_fx33();
            break;
        case 0x55:
            oc_fx55();
            break;
        case 0x65:
            oc_fx65();
            break;
    }
}