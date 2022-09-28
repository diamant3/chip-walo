#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "system.h"

static const uint8_t font_set[FONT_LENGTH] = {
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
    srand((uint32_t)time(NULL));
    cw->I = 0;
    cw->opcode = 0;
    cw->SP = 0;
    memset(cw->gfx, 0, SCREEN_SIZE);
    memset(cw->stack, 0, STACK_SIZE);
    memset(cw->keypad, 0, 16);
    memset(cw->reg_v, 0, REGISTER_COUNT);
    cw->PC = MEMORY_ADDRESS_START;

    for (uint32_t font = 0; font < FONT_LENGTH; ++font) {
        cw->mem[font] = font_set[font];
    }
}

void core_load(Chip_walo *cw, const uint8_t *rom) {
    uint64_t rom_size;
    uint8_t *rom_buffer = NULL;
    FILE *file = NULL;

    // open the rom file
    file = fopen(rom, "rb");
    if (file != NULL) {
        // Get the rom size & buffer
        fseek(file, 0, SEEK_END);
        rom_size = ftell(file);
        printf("ROM Size: %lld Kb\n", rom_size);
        rewind(file);
        rom_buffer = (uint8_t *) malloc(rom_size * (sizeof(rom_buffer[0])));
    } else {
        fprintf(stderr, "Opening file failed, Error: %s\n", strerror(errno));
        return;
    }

    // Check and Load the rom to memory
    size_t status = fread(rom_buffer, 1, rom_size, file);
    if (status > 0) {
        if (rom_size <= (MEMORY_SIZE - MEMORY_ADDRESS_START)) {
            for (uint32_t data = 0; data < rom_size; ++data) {
                cw->mem[MEMORY_ADDRESS_START + data] = rom_buffer[data];
            }
        } else {
            printf("Loading rom failed, Error: rom size exceeded.\n");
            return;
        }
    } else {
        printf("Loading rom failed, Error: failed to read the file.\n");
        return;
    }

    free(rom_buffer);
    fclose(file);
}

void core_cycle(Chip_walo *cw) {
    cw->opcode = (cw->mem[cw->PC] << 8) | cw->mem[cw->PC + 1];
    
    switch (cw->opcode & 0xF000) {
        case 0x0000:
            switch (BYTE) {
                // Clear the display
                case 0xE0:
                    memset(cw->gfx, 0, SCREEN_SIZE);
                    cw->draw_flag = true;
                    cw->PC += 2;
                break;

                // Return from a subroutine
                case 0xEE:
                    cw->PC = cw->stack[--cw->SP];
                    cw->PC += 2;
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", cw->opcode);
                break;
            }
            break;
        break;

        // Jump to location ADDR
        case 0x1000:
            cw->PC = ADDR;
        break;

        // Call a subroutine at ADDR
        case 0x2000:
            cw->stack[cw->SP] = cw->PC;
            ++cw->SP;
            cw->PC = ADDR;
        break;

        // Skip next instruction if register Vx == byte
        case 0x3000:
            if (cw->reg_v[X] == BYTE) {
                cw->PC += 4;
            } else {
                cw->PC += 2;
            }
        break;

        // Skip next instruction if register Vx != byte
        case 0x4000:
            if (cw->reg_v[X] != BYTE) {
                cw->PC += 4;
            } else {
                cw->PC += 2;
            }
        break;

        // Skip next instruction if register Vx == register Vy
        case 0x5000:
            if (cw->reg_v[X] == cw->reg_v[Y]) {
                cw->PC += 4;
            } else {
                cw->PC += 2;
            }
        break;

        // Set register Vx = byte
        case 0x6000:
            cw->reg_v[X] = BYTE;
            cw->PC += 2;
        break;

       // Set register Vx = Vx + byte
        case 0x7000:
            cw->reg_v[X] += BYTE;
            cw->PC += 2;
        break;

        case 0x8000:
            switch (NIBBLE)
            {
                // Set register Vx = Vy
                case 0x0:
                    cw->reg_v[X] = cw->reg_v[Y];
                    cw->PC += 2;
                break;

                // Set register Vx = Vx OR Vy
                case 0x1:
                    cw->reg_v[X] |= cw->reg_v[Y];
                    cw->PC += 2;
                break;

                // Set register Vx = Vx AND Vy
                case 0x2:
                    cw->reg_v[X] &= cw->reg_v[Y];
                    cw->PC += 2;
                break;

                // Set register Vx = Vx XOR Vy
                case 0x3:
                    cw->reg_v[X] ^= cw->reg_v[Y];
                    cw->PC += 2;
                break;

                // Set register Vx = Vx + Vy, Set register Vf = 1 (Carry)
                case 0x4: {
                    uint16_t result = cw->reg_v[X] + cw->reg_v[Y];

                    if (result > 0xFF) {
                        cw->reg_v[0xF] = true;
                    } else {
                        cw->reg_v[0xF] = false;
                    }
                    cw->reg_v[X] = (result & 0xFF);
                    cw->PC += 2;
                }
                break;

                // Set register Vx = Vx - Vy, Set register Vf = NOT borrow
                case 0x5: {
                    uint8_t result = cw->reg_v[X] - cw->reg_v[Y];

                    if (cw->reg_v[X] > cw->reg_v[Y]) {
                        cw->reg_v[0xF] = true;
                    } else {
                        cw->reg_v[0xF] = false;
                    }
                    cw->reg_v[X] = result;
                    cw->PC += 2;
                }
                break;

                // Set register Vx = Vx SHR 1
                case 0x6:
                    cw->reg_v[X] >>= 1;
                    cw->reg_v[0xF] = (cw->reg_v[X] & 8) ? false : true;
                    cw->PC += 2;
                break;

                // Set register Vx = Vy - Vx, Set register Vf = NOT borrow
                case 0x7: {
                    uint8_t result = cw->reg_v[Y] - cw->reg_v[X];

                    if (cw->reg_v[Y] > cw->reg_v[X]) {
                        cw->reg_v[0xF] = true;
                    } else {
                        cw->reg_v[0xF] = false;
                    }
                    cw->reg_v[X] = result;
                    cw->PC += 2;
                }
                break;

                // Set register Vx = Vx SHL 1
                case 0xE:
                    cw->reg_v[X] <<= 1;
                    cw->reg_v[0xF] = (cw->reg_v[X] >> 7) ? true : false;
                    cw->PC += 2;
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", cw->opcode);
                break;
            }
            break;

        // Skip next instruction if register Vx != Vy
        case 0x9000:
            if (cw->reg_v[X] != cw->reg_v[Y]) {
                cw->PC += 4;
            } else {
                cw->PC += 2;
            }
        break;

        // Set I = NNN
        case 0xA000:
            cw->I = ADDR;
            cw->PC += 2;
        break;

        // Jump to location NNN + register V0
        case 0xB000:
            cw->PC = (ADDR + cw->reg_v[0]);
        break;

        // Set register Vx = random byte AND byte
        case 0xC000: {
            uint8_t random = (rand() % 256);
            cw->reg_v[X] = (random & BYTE);
            cw->PC += 2;
        }
        break;

        // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
        case 0xD000: {
                uint8_t x = cw->reg_v[X];
                uint8_t y = cw->reg_v[Y];
                uint8_t height = NIBBLE;
                uint32_t px = 0;
                cw->reg_v[0xF] = false;

                for (size_t yline = 0; yline < height; ++yline) {
                    px = cw->mem[cw->I + yline];
                    for (size_t xline = 0; xline < 8; ++xline) {
                        if ((px & (0x80 >> xline)) != 0) {
                            if (cw->gfx[x + xline + ((y + yline) * SCREEN_WIDTH) % (SCREEN_WIDTH * SCREEN_HEIGHT)] == true) {
                                cw->reg_v[0xF] = true; // Collision
                            }
                            cw->gfx[((x + xline) + ((y + yline) * SCREEN_WIDTH))] ^= 1;
                        }
                    }
                }
                cw->draw_flag = true;
                cw->PC += 2;
            }
        break;

        case 0xE000:
            switch(BYTE) {
                // Skip next instruction if key with the value of register Vx is pressed
                case 0x9E:
                    if (cw->keypad[cw->reg_v[X]] == true) {
                        cw->PC += 4;
                    } else {
                        cw->PC += 2;
                    }
                break;

                // Skip next instruction if key with the value of Vx is not pressed
                case 0xA1:
                    if (cw->keypad[cw->reg_v[X]] == false) {
                        cw->PC += 4;
                    } else {
                        cw->PC += 2;
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
                    cw->reg_v[X] = cw->DT;
                    cw->PC += 2;
                break;

                // Wait for a key press, store the value of the key in register Vx
                case 0x0A:
                    for (uint8_t key = 0; key < 16; ++key) {
                        if (cw->keypad[key] == true) {
                            cw->reg_v[X] = key;
                            cw->PC += 2;
                            break;
                        }
                    }
                break;

                // Set delay timer = register Vx
                case 0x15:
                    cw->DT = cw->reg_v[X];
                    cw->PC += 2;
                break;

                // Set sound timer = Vx
                case 0x18:
                    cw->ST = cw->reg_v[X];
                    cw->PC += 2;
                break;

                // Set I = I + register Vx
                case 0x1E:
                    cw->I += cw->reg_v[X];
                    cw->PC += 2;
                break;

                // Set I = location of sprite for digit register Vx
                case 0x29:
                    cw->I = (cw->reg_v[X] * 5);
                    cw->PC += 2;
                break;

                // Store BCD representation of register Vx in memory locations I, I+1, and I+2
                case 0x33:
                    cw->mem[cw->I] = (cw->reg_v[X] / 100);
                    cw->mem[cw->I + 1] = (cw->reg_v[X] / 10) % 10;
                    cw->mem[cw->I + 2] = (cw->reg_v[X] % 100) % 10;
                    cw->PC += 2;
                break;

                // Store registers V0 through Vx in memory starting at location I
                case 0x55:
                    for (uint32_t v = 0; v <= X; ++v) {
                        cw->mem[cw->I + v] = cw->reg_v[v];
                    }
                    cw->PC += 2;
                break;

                // Read registers V0 through Vx from memory starting at location I
                case 0x65:
                    for (uint32_t v = 0;v <= X; ++v) {
                        cw->reg_v[v] = cw->mem[cw->I + v];
                    }
                    cw->PC += 2;
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", cw->opcode);
                break;
            }
        break;
    }

    // Update Timers
    if (cw->DT > 0) {
        --cw->DT;
    }

    if (cw->ST > 0) {
        --cw->ST;
        cw->audio_flag = true;
    }
}