#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "system.h"

const unsigned char FONTSET[FONT_LENGTH] = 
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

char running = 1;

// milisecond delay
static void mdelay(int sec)
{
    int millis = sec / 1000;
    clock_t start = clock();
    while(clock() <= (start + millis));
}

void core_init(void)
{
    chip_walo = (Chip_walo *)malloc(sizeof(*chip_walo));

    /* Generate a random seed */
    srand((unsigned int)time(NULL));

    /* Set all to zero */
    chip_walo->i = 0;
    chip_walo->opcode = 0;
    chip_walo->sp = 0;
    memset(chip_walo->mem, 0, MEMORY_SIZE);
    memset(chip_walo->gfx, 0, SCREEN_SIZE);
    memset(chip_walo->stack, 0, STACK_SIZE);
    memset(chip_walo->reg_v, 0, REGISTER_COUNT);

    /* Load font to memory */
    for (int idx = 0; idx < FONT_LENGTH; idx++)
        chip_walo->mem[START_FONT_ADDRESS + idx] = FONTSET[idx];

    /* Jump to chip-8 program's address space */
    chip_walo->pc = START_MEM_ADDRESS;
    printf("[SUCCESS] chip-walo Initialized.\n");
}

void core_deinit(void)
{
    free(chip_walo);
    chip_walo = NULL;
    printf("[SUCCESS] chip-walo Deinitialized.\n");
}

void core_load(const char *rom) 
{
    unsigned long rom_size = 0;
    unsigned char *rom_buffer = NULL;
    FILE *file = NULL;

    /* Get size and allocate a ROM to buffer */
    file = fopen(rom, "rb");
    if (file != NULL) 
    {
        fseek(file, 0, SEEK_END);
        rom_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        rom_buffer = (unsigned char *)malloc(rom_size * (sizeof(rom_buffer[0])));
    } 
    else 
    {
        printf("[FAIL] Can't open the file.\n");
        exit(0);
    }

    /* Check and Load the ROM to memory */
    if (fread(rom_buffer, 1, rom_size, file) > 0) 
    {
        if (rom_size <= (MEMORY_SIZE - START_MEM_ADDRESS)) 
        {
            for (int idx = 0; idx < rom_size; idx++)
                chip_walo->mem[START_MEM_ADDRESS + idx] = rom_buffer[idx];
        } 
        else 
        {
            printf("[FAIL] File size exceeded.\n");
            exit(0);
        }
    } 
    else 
    {
        printf("[FAIL] Empty file.\n");
        exit(0);
    }

    free(rom_buffer);
    fclose(file);
    printf("[SUCCESS] ROM loaded.\n");
}
void core_cycle(void)
{
    chip_walo->opcode = (chip_walo->mem[chip_walo->pc] << 8) | chip_walo->mem[chip_walo->pc + 1];
    printf("[STATE] opcode = %x \r", chip_walo->opcode);

    switch (chip_walo->opcode & 0xF000) 
    {
        case 0x0000:
            switch (BYTE) 
            {
                // Clear the display 
                case 0xE0:
                    mdelay(109);
                    memset(chip_walo->gfx, 0, SCREEN_SIZE);
                    chip_walo->draw_flag = 1;
                    chip_walo->pc += 2;
                break;
                
                // Return from a subroutine 
                case 0xEE:
                    mdelay(105);
                    chip_walo->pc = chip_walo->stack[chip_walo->sp];
                    chip_walo->stack[chip_walo->sp] = 0;
                    chip_walo->sp--;
                    chip_walo->pc += 2;
                break;
     
                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", chip_walo->opcode);
                break;
            }
        break;

        // Jump to location ADDR
        case 0x1000:
            mdelay(105);
            chip_walo->pc = ADDR;
        break;

        // Call a subroutine at ADDR
        case 0x2000:
            mdelay(105);
            chip_walo->sp++;
            chip_walo->stack[chip_walo->sp] = chip_walo->pc;
            chip_walo->pc = ADDR;
        break;

        // Skip next instruction if register Vx == byte 
        case 0x3000:
            mdelay(55);
            if (chip_walo->reg_v[X] == BYTE) 
                chip_walo->pc += 4;
            else
                chip_walo->pc += 2;
        break;

        // Skip next instruction if register Vx != byte 
        case 0x4000:
            mdelay(55);
            if (chip_walo->reg_v[X] != BYTE) 
                chip_walo->pc += 4;
            else 
                chip_walo->pc += 2;
        break;

        // Skip next instruction if register Vx == register Vy 
        case 0x5000:
            mdelay(73);
            if (chip_walo->reg_v[X] == chip_walo->reg_v[Y]) 
                chip_walo->pc += 4;
            else
                chip_walo->pc += 2;
        break;

        // Set register Vx = byte 
        case 0x6000:
            mdelay(27);
            chip_walo->reg_v[X] = BYTE;
            chip_walo->pc += 2;
        break;

        // Set register Vx = Vx + byte 
        case 0x7000:
            mdelay(45);
            chip_walo->reg_v[X] += BYTE;
            chip_walo->pc += 2;
        break;

        case 0x8000:
            switch (NIBBLE) 
            {
                // Set register Vx = Vy
                case 0x0:
                    mdelay(200);
                    chip_walo->reg_v[X] = chip_walo->reg_v[Y];
                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vx OR Vy
                case 0x1:
                    mdelay(200);
                    chip_walo->reg_v[X] |= chip_walo->reg_v[Y];
                    chip_walo->reg_v[0xF] = 0;

                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vx AND Vy
                case 0x2:
                    mdelay(200);
                    chip_walo->reg_v[X] &= chip_walo->reg_v[Y];
                    chip_walo->reg_v[0xF] = 0;

                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vx XOR Vy
                case 0x3:
                    mdelay(200);
                    chip_walo->reg_v[X] ^= chip_walo->reg_v[Y];
                    chip_walo->reg_v[0xF] = 0;

                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vx + Vy, Set register Vf = 1(Carry)
                case 0x4 : 
                {
                    mdelay(200);
                    unsigned short result = chip_walo->reg_v[X] + chip_walo->reg_v[Y];
                    if (result > 0xFF) 
                        chip_walo->reg_v[0xF] = 1;
                    else
                        chip_walo->reg_v[0xF] = 0;

                    chip_walo->reg_v[X] = (result & 0xFF);
                    chip_walo->pc += 2;
                } 
                break;

                // Set register Vx = Vx - Vy, Set register Vf = NOT borrow
                case 0x5 : 
                {
                    mdelay(200);
                    unsigned char result = chip_walo->reg_v[X] - chip_walo->reg_v[Y];
                    if (chip_walo->reg_v[Y] > chip_walo->reg_v[X])
                        chip_walo->reg_v[0xF] = 0;
                    else
                        chip_walo->reg_v[0xF] = 1;

                    chip_walo->reg_v[X] = result;
                    chip_walo->pc += 2;
                }
                break;

                // Set register Vx = Vx SHR 1
                case 0x6:
                    mdelay(200);
                    chip_walo->reg_v[X] >>= 1;
                    chip_walo->reg_v[0xF] = (chip_walo->reg_v[X] & 8) ? 0 : 1;
                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vy - Vx, Set register Vf = NOT borrow
                case 0x7 : 
                {
                    mdelay(200);
                    unsigned char result = chip_walo->reg_v[Y] - chip_walo->reg_v[X];
                    if (chip_walo->reg_v[X] > chip_walo->reg_v[Y])
                        chip_walo->reg_v[0xF] = 0;
                    else
                        chip_walo->reg_v[0xF] = 1;

                    chip_walo->reg_v[X] = result;
                    chip_walo->pc += 2;
                } 
                break;

                // Set register Vx = Vx SHL 1
                case 0xE:
                    mdelay(200);
                    chip_walo->reg_v[X] <<= 1;
                    chip_walo->reg_v[0xF] = (chip_walo->reg_v[X] >> 7) ? 1 : 0;
                    chip_walo->pc += 2;
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", chip_walo->opcode);
                break;
            }
        break;

        // Skip next instruction if register Vx != Vy
        case 0x9000:
            mdelay(73);
            if (chip_walo->reg_v[X] != chip_walo->reg_v[Y])
                chip_walo->pc += 4;
            else
                chip_walo->pc += 2;
        break;

        // Set I = NNN
        case 0xA000:
            mdelay(55);
            chip_walo->i = ADDR;
            chip_walo->pc += 2;
        break;

        // Jump to location NNN + register Vx
        case 0xB000:
            mdelay(105);
            chip_walo->pc = (ADDR + chip_walo->reg_v[X]);
        break;

        // Set register Vx = random byte AND byte
        case 0xC000: 
        {
            mdelay(164);
            unsigned char random = (rand() % 0xFF);
            chip_walo->reg_v[X] = (random & BYTE);
            chip_walo->pc += 2;
        }
        break;
        
        // Display n - byte sprite starting at memory location I at(Vx, Vy), set VF = collision
        case 0xD000: 
        {
            mdelay(22734);
            unsigned char x = chip_walo->reg_v[X] % SCREEN_WIDTH;
            unsigned char y = chip_walo->reg_v[Y] % SCREEN_HEIGHT;
            unsigned char height = NIBBLE;
            unsigned char px = 0;
            chip_walo->reg_v[0xF] = 0;

            for (unsigned char yline = 0; yline < height; yline++) 
            {
                px = chip_walo->mem[chip_walo->i + yline];
                for (unsigned char xline = 0; xline < 8; xline++) 
                {
                    if ((px & (0x80 >> xline))) 
                    {
                        if (chip_walo->gfx[((x + xline) + ((y + yline) * SCREEN_WIDTH))] == 1) 
                            chip_walo->reg_v[0xF] = 1; // collision

                        chip_walo->gfx[((x + xline) + ((y + yline) * SCREEN_WIDTH))] ^= 1;
                    }
                }
            }
            
            chip_walo->draw_flag = 1;
            chip_walo->pc += 2;
        } 
        break;

        case 0xE000:
            switch (BYTE) 
            {
                // Skip next instruction if key with the value of register Vx is pressed
                case 0x9E:
                    mdelay(73);
                    if (chip_walo->key[chip_walo->reg_v[X]] == 1)
                        chip_walo->pc += 4;
                    else
                        chip_walo->pc += 2;
                break;

                // Skip next instruction if key with the value of Vx is not pressed 
                case 0xA1:
                    mdelay(73);
                    if (chip_walo->key[chip_walo->reg_v[X]] == 0)
                        chip_walo->pc += 4;
                    else
                        chip_walo->pc += 2;
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", chip_walo->opcode);
                break;
            }
        break;

        case 0xF000:
            switch (BYTE) 
            {
                // Set register Vx = delay timer value
                case 0x07:
                    mdelay(45);
                    chip_walo->reg_v[X] = chip_walo->dt;
                    chip_walo->pc += 2;
                break;  
                
                // Wait for a key press, store the value of the key in register Vx 
                case 0x0A:
                    for (unsigned char key = 0; key < 16; key++) 
                    {
                        if (chip_walo->key[key] == 1) 
                        {
                            chip_walo->reg_v[X] = key;
                            chip_walo->pc += 2;
                            break;
                        }
                    }
                break;

                // Set delay timer = register Vx
                case 0x15:
                    mdelay(45);
                    chip_walo->dt = chip_walo->reg_v[X];
                    chip_walo->pc += 2;
                break;

                // Set sound timer = Vx
                case 0x18:
                    mdelay(45);
                    chip_walo->st = chip_walo->reg_v[X];
                    chip_walo->pc += 2;
                break;
                
                // Set I = I + register Vx
                case 0x1E:
                    mdelay(86);
                    chip_walo->i += chip_walo->reg_v[X];
                    chip_walo->pc += 2;
                break;

                // Set I = location of sprite for digit register Vx
                case 0x29:
                    mdelay(91);
                    chip_walo->i = (chip_walo->reg_v[X] * 5);
                    chip_walo->pc += 2;
                break;
  
                // Store BCD representation of register Vx in memory locations I,I + 1, and I + 2
                case 0x33:
                    mdelay(927);
                    chip_walo->mem[chip_walo->i] = (chip_walo->reg_v[X] / 100);
                    chip_walo->mem[chip_walo->i + 1] = (chip_walo->reg_v[X] / 10) % 10;
                    chip_walo->mem[chip_walo->i + 2] = (chip_walo->reg_v[X] % 100) % 10;
                    chip_walo->pc += 2;
                break;

                // Store registers V0 through Vx in memory starting at location I
                case 0x55:
                    mdelay(605);
                    for (unsigned int v = 0; v <= X; v++)
                        chip_walo->mem[chip_walo->i + v] = chip_walo->reg_v[v];

                    chip_walo->pc += 2;
                break;

                // Read registers V0 through Vx from memory starting at location I
                case 0x65:
                    mdelay(605);
                    for (unsigned int v = 0; v <= X; v++)
                        chip_walo->reg_v[v] = chip_walo->mem[chip_walo->i + v];

                    chip_walo->pc += 2;
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", chip_walo->opcode);
                break;
            }
        break;
    }

    // Update Timers
    if (chip_walo->dt > 0) 
        chip_walo->dt--;

    if (chip_walo->st > 0) 
    {
        chip_walo->st--;
        chip_walo->audio_flag = 1;
    }
}