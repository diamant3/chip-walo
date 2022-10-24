#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/system.h"

const unsigned char FONTSET[FONT_LENGTH] = {
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

void core_init(Chip_walo *cw) {
    srand((unsigned int)time(NULL));
    cw->i = 0;
    cw->opcode = 0;
    cw->sp = 0;
    memset(cw->mem, 0, MEMORY_SIZE);
    memset(cw->gfx, 0, SCREEN_SIZE);
    memset(cw->stack, 0, STACK_SIZE);
    memset(cw->key, 0, 16);
    memset(cw->reg_v, 0, REGISTER_COUNT);
    for (int idx = 0; idx < FONT_LENGTH; idx++) {
        cw->mem[START_FONT_ADDRESS] = FONTSET[idx];
    }
    cw->pc = START_MEM_ADDRESS;
}

int core_load(Chip_walo *cw, const char *rom) {
    long rom_size = 0;
    unsigned char *rom_buffer = NULL;
    FILE *file = NULL;
    if (fopen_s(&file, rom, "b") > 0) {
        fseek(file, 0, SEEK_END);
        rom_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        rom_buffer = (unsigned char *)malloc(rom_size * (sizeof(rom_buffer[0])));
    } else {
        return 1;
    }

    // Check and Load the rom to memory
    if (fread(rom_buffer, 1, rom_size, file) > 0) {
        if (rom_size <= (MEMORY_SIZE - START_MEM_ADDRESS)) {
            for (int idx = 0; idx < rom_size; idx++) {
                cw->mem[START_MEM_ADDRESS + idx] = rom_buffer[idx];
            }
        } else {
            printf("Error: File size exceeded.\n");
            return 1;
        }
    } else {
        printf("Error: Failed to read the file.\n");
        return 1;
    }

    free(rom_buffer);
    fclose(file);
    return 0;
}
void core_cycle(Chip_walo *cw) {
    cw->opcode = (cw->mem[cw->pc] << 8) | cw->mem[cw->pc + 1];

    switch (cw->opcode & 0xF000) {
        case 0x0000:
            switch (BYTE) {
                // Clear the display 
                case 0xE0:
                    memset(cw->gfx, 0, SCREEN_SIZE);
                    cw->draw_flag = 1;
                    cw->pc += 2;
                break;
                
                // Return from a subroutine 
                case 0xEE:
                    cw->pc = cw->stack[--cw->sp];
                    cw->pc += 2;
                break;
     
                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", cw->opcode);
                break;
            }
        break;

        // Jump to location ADDR
        case 0x1000:
            cw->pc = ADDR;
        break;

        // Call a subroutine at ADDR
        case 0x2000:
            cw->stack[cw->sp] = cw->pc;
            cw->sp++;
            cw->pc = ADDR;
        break;

        // Skip next instruction if register Vx == byte 
        case 0x3000:
            if (cw->reg_v[X] == BYTE) {
                cw->pc += 4;
            } else {
                cw->pc += 2;
            }
        break;

        // Skip next instruction if register Vx != byte 
        case 0x4000:
            if (cw->reg_v[X] != BYTE) {
                cw->pc += 4;
            } else {
                cw->pc += 2;
            }
        break;

        // Skip next instruction if register Vx == register Vy 
        case 0x5000:
            if (cw->reg_v[X] == cw->reg_v[Y]) {
                cw->pc += 4;
            } else {
                cw->pc += 2;
            }
        break;

        // Set register Vx = byte 
        case 0x6000:
            cw->reg_v[X] = BYTE;
            cw->pc += 2;
        break;

        // Set register Vx = Vx + byte 
        case 0x7000:
            cw->reg_v[X] += BYTE;
            cw->pc += 2;
        break;

        case 0x8000:
            switch (NIBBLE) {
                // Set register Vx = Vy
                case 0x0:
                    cw->reg_v[X] = cw->reg_v[Y];
                    cw->pc += 2;
                break;

                // Set register Vx = Vx OR Vy
                case 0x1:
                    cw->reg_v[X] |= cw->reg_v[Y];
                    cw->pc += 2;
                break;

                // Set register Vx = Vx AND Vy
                case 0x2:
                    cw->reg_v[X] &= cw->reg_v[Y];
                    cw->pc += 2;
                break;

                // Set register Vx = Vx XOR Vy
                case 0x3:
                    cw->reg_v[X] ^= cw->reg_v[Y];
                    cw->pc += 2;
                break;

                // Set register Vx = Vx + Vy, Set register Vf = 1(Carry)
                case 0x4 : {
                    unsigned short result = cw->reg_v[X] + cw->reg_v[Y];
                    if (result > 0xFF) {
                        cw->reg_v[0xF] = 1;
                    } else {
                        cw->reg_v[0xF] = 0;
                    }
                    cw->reg_v[X] = (result & 0xFF);
                    cw->pc += 2;
                } 
                break;

                // Set register Vx = Vx - Vy, Set register Vf = NOT borrow
                case 0x5 : {
                    unsigned char result = cw->reg_v[X] - cw->reg_v[Y];
                    if (cw->reg_v[X] > cw->reg_v[Y]) {
                        cw->reg_v[0xF] = 1;
                    } else {
                        cw->reg_v[0xF] = 0;
                    }
                    cw->reg_v[X] = result;
                    cw->pc += 2;
                }
                break;

                // Set register Vx = Vx SHR 1
                case 0x6:
                    cw->reg_v[X] >>= 1;
                    cw->reg_v[0xF] = (cw->reg_v[X] & 8) ? 0 : 1;
                    cw->pc += 2;
                break;

                // Set register Vx = Vy - Vx, Set register Vf = NOT borrow
                case 0x7 : {
                    unsigned char result = cw->reg_v[Y] - cw->reg_v[X];
                    if (cw->reg_v[Y] > cw->reg_v[X]) {
                        cw->reg_v[0xF] = 1;
                    } else {
                        cw->reg_v[0xF] = 0;
                    }
                    cw->reg_v[X] = result;
                    cw->pc += 2;
                } 
                break;

                // Set register Vx = Vx SHL 1
                case 0xE:
                    cw->reg_v[X] <<= 1;
                    cw->reg_v[0xF] = (cw->reg_v[X] >> 7) ? 1 : 0;
                    cw->pc += 2;
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", cw->opcode);
                break;
            }
        break;

        // Skip next instruction if register Vx != Vy
        case 0x9000:
            if (cw->reg_v[X] != cw->reg_v[Y]) {
                cw->pc += 4;
            } else {
                cw->pc += 2;
            }
        break;

        // Set I = NNN
        case 0xA000:
            cw->i = ADDR;
            cw->pc += 2;
        break;

        // Jump to location NNN + register V0
        case 0xB000:
            cw->pc = (ADDR + cw->reg_v[0]);
        break;

        // Set register Vx = random byte AND byte
        case 0xC000 : {
            unsigned char random = (rand() % 256);
            cw->reg_v[X] = (random & BYTE);
            cw->pc += 2;
        }
        break;
        
        // Display n - byte sprite starting at memory location I at(Vx, Vy), set VF = collision
        case 0xD000 : {
            unsigned char x = cw->reg_v[X];
            unsigned char y = cw->reg_v[Y];
            unsigned char height = NIBBLE;
            unsigned int px = 0;
            cw->reg_v[0xF] = 0;

            for (size_t yline = 0; yline < height; ++yline) {
                px = cw->mem[cw->i + yline];
                for (size_t xline = 0; xline < 8; ++xline) {
                    int px_data = (px & (0x80 >> xline));
                    unsigned char px_offset = cw->gfx[x + xline + ((y + yline) * SCREEN_WIDTH) % (SCREEN_WIDTH * SCREEN_HEIGHT)]
                    if (px_data != 0) {
                        if (px_offset == 1) {
                            cw->reg_v[0xF] = 1; // collision
                        }
                        cw->gfx[((x + xline) + ((y + yline) * SCREEN_WIDTH))] ^= 1;
                    }
                }
            }
            
            cw->draw_flag = 1;
            cw->pc += 2;
        } 
        break;

        case 0xE000:
            switch (BYTE) {
                // Skip next instruction if key with the value of register Vx is pressed
                case 0x9E:
                    if (cw->key[cw->reg_v[X]] == 1) {
                        cw->pc += 4;
                    } else {
                        cw->pc += 2;
                    }
                break;

                // Skip next instruction if key with the value of Vx is not pressed 
                case 0xA1:
                    if (cw->key[cw->reg_v[X]] == 0) {
                        cw->pc += 4;
                    } else {
                        cw->pc += 2;
                    }
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", cw->opcode);
                break;
            }
        break;

        case 0xF000:
            switch (BYTE) {
                // Set register Vx = delay timer value
                case 0x07:
                    cw->reg_v[X] = cw->dt;
                    cw->pc += 2;
                break;  
                
                // Wait for a key press, store the value of the key in register Vx 
                case 0x0A:
                    for (unsigned char key = 0; key < 16; ++key) {
                        if (cw->key[key] == 1) {
                            cw->reg_v[X] = key;
                            cw->pc += 2;
                            break;
                        }
                    }
                break;

                // Set delay timer = register Vx
                case 0x15:
                    cw->dt = cw->reg_v[X];
                    cw->pc += 2;
                break;

                // Set sound timer = Vx
                case 0x18:
                    cw->st = cw->reg_v[X];
                    cw->pc += 2;
                break;
                
                // Set I = I + register Vx
                case 0x1E:
                    cw->i += cw->reg_v[X];
                    cw->pc += 2;
                break;

                // Set I = location of sprite for digit register Vx
                case 0x29:
                    cw->i = (cw->reg_v[X] * 5);
                    cw->pc += 2;
                break;
  
                // Store BCD representation of register Vx in memory locations I,I + 1, and I + 2
                case 0x33:
                    cw->mem[cw->i] = (cw->reg_v[X] / 100);
                    cw->mem[cw->i + 1] = (cw->reg_v[X] / 10) % 10;
                    cw->mem[cw->i + 2] = (cw->reg_v[X] % 100) % 10;
                    cw->pc += 2;
                break;

                // Store registers V0 through Vx in memory starting at location I
                case 0x55:
                    for (unsigned int v = 0; v <= X; ++v) {
                        cw->mem[cw->i + v] = cw->reg_v[v];
                    }
                    cw->pc += 2;
                break;

                // Read registers V0 through Vx from memory starting at location I
                case 0x65:
                    for (unsigned int v = 0; v <= X; ++v) {
                        cw->reg_v[v] = cw->mem[cw->i + v];
                    }
                    cw->pc += 2;
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", cw->opcode);
                break;
            }
        break;
    }

    // Update Timers
    if (cw->dt > 0) { cw->dt--; }
    if (cw->st > 0) {
        cw->st--;
        cw->audio_flag = 1;
    }
}