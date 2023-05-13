#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "system.h"

const uint8_t FONTSET[FONT_LENGTH] = {
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

uint8_t running = 1;
Chip_walo *chip_walo = NULL;

void core_init(void) {
    chip_walo = calloc(1, sizeof(*chip_walo));

    /* Generate a random seed */
    srand(time(NULL));

    /* Load font to memory */
    memcpy(&chip_walo->mem[START_FONT_ADDRESS], FONTSET, FONT_LENGTH);

    /* Jump to chip-8 program's address space */
    chip_walo->pc = START_MEM_ADDRESS;

    printf("[SUCCESS] chip-walo Initialized.\n");
}

void core_deinit(void) {
    free(chip_walo);
    chip_walo = NULL;
    printf("[SUCCESS] chip-walo Deinitialized.\n");
}

void core_load(const char *rom) {
    uint64_t rom_size = 0;
    uint8_t *rom_buffer = NULL;
    FILE *file = NULL;

    /* Open the file */
    file = fopen(rom, "rb");
    if (file == NULL) {
        printf("[ERROR] Failed to open the file.\n");
        exit(EXIT_FAILURE);
    }

    /* Get the file size */
    if (fseek(file, 0, SEEK_END) != 0) {
        printf("[ERROR] Failed to seek to end of file.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    rom_size = ftell(file);
    if (rom_size == -1L) {
        printf("[ERROR] Failed to get the file size.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (rom_size > (MEMORY_SIZE - START_FONT_ADDRESS)) {
        printf("[ERROR] File size exceeds available memory. \n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        printf("[ERROR] Failed to seek to beginning of file. \n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    /* Allocate memory for the ROM buffer */
    rom_buffer = malloc(rom_size);
    if (rom_buffer == NULL) {
        printf("[ERROR] Failed to allocate memory for ROM buffer. \n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    /* Read the file into the ROM buffer */
    if (fread(rom_buffer, 1, rom_size, file) != rom_size) {
        printf("[ERROR] Failed to read ROM data from file. \n");
        fclose(file);
        free(rom_buffer);
        exit(EXIT_FAILURE);
    }

    /* Copy the ROM data to the Chip-8 memory */
    memcpy(&chip_walo->mem[START_MEM_ADDRESS], rom_buffer, rom_size);

    /* Clean up and print success message */
    free(rom_buffer);
    fclose(file);
    printf("[SUCCESS] ROM loaded. \n");
}

void core_cycle(void) {
    chip_walo->opcode = (chip_walo->mem[chip_walo->pc] << 8) | chip_walo->mem[chip_walo->pc + 1];
    printf("[STATE] opcode = %x \r", chip_walo->opcode);

    switch (chip_walo->opcode & 0xF000) {
        case 0x0000:
            switch (BYTE) {
                // Clear the display 
                case 0xE0:
                    memset(chip_walo->gfx, 0, SCREEN_SIZE);
                    chip_walo->draw_flag = 1;
                    chip_walo->pc += 2;
                break;
                
                // Return from a subroutine 
                case 0xEE:
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
            chip_walo->pc = ADDR;
        break;

        // Call a subroutine at ADDR
        case 0x2000:
            chip_walo->sp++;
            chip_walo->stack[chip_walo->sp] = chip_walo->pc;
            chip_walo->pc = ADDR;
        break;

        // Skip next instruction if register Vx == byte 
        case 0x3000:
            if (chip_walo->reg_v[X] == BYTE) 
                chip_walo->pc += 4;
            else
                chip_walo->pc += 2;
        break;

        // Skip next instruction if register Vx != byte 
        case 0x4000:
            if (chip_walo->reg_v[X] != BYTE) 
                chip_walo->pc += 4;
            else 
                chip_walo->pc += 2;
        break;

        // Skip next instruction if register Vx == register Vy 
        case 0x5000:
            if (chip_walo->reg_v[X] == chip_walo->reg_v[Y]) 
                chip_walo->pc += 4;
            else
                chip_walo->pc += 2;
        break;

        // Set register Vx = byte 
        case 0x6000:
            chip_walo->reg_v[X] = BYTE;
            chip_walo->pc += 2;
        break;

        // Set register Vx = Vx + byte 
        case 0x7000:
            chip_walo->reg_v[X] += BYTE;
            chip_walo->pc += 2;
        break;

        case 0x8000:
            switch (NIBBLE) {
                // Set register Vx = Vy
                case 0x0:
                    chip_walo->reg_v[X] = chip_walo->reg_v[Y];
                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vx OR Vy
                case 0x1:
                    chip_walo->reg_v[X] |= chip_walo->reg_v[Y];
                    chip_walo->reg_v[0xF] = 0;

                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vx AND Vy
                case 0x2:
                    chip_walo->reg_v[X] &= chip_walo->reg_v[Y];
                    chip_walo->reg_v[0xF] = 0;

                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vx XOR Vy
                case 0x3:
                    chip_walo->reg_v[X] ^= chip_walo->reg_v[Y];
                    chip_walo->reg_v[0xF] = 0;

                    chip_walo->pc += 2;
                break;

                // Set register Vx = Vx + Vy, Set register Vf = 1(Carry)
                case 0x4: {
                    uint16_t result = chip_walo->reg_v[X] + chip_walo->reg_v[Y];
                    if (result > 0xFF) 
                        chip_walo->reg_v[0xF] = 1;
                    else
                        chip_walo->reg_v[0xF] = 0;

                    chip_walo->reg_v[X] = (result & 0xFF);
                    chip_walo->pc += 2;
                } 
                break;

                // Set register Vx = Vx - Vy, Set register Vf = NOT borrow
                case 0x5: {
                    uint8_t result = chip_walo->reg_v[X] - chip_walo->reg_v[Y];
                    if (chip_walo->reg_v[X] >= chip_walo->reg_v[Y])
                        chip_walo->reg_v[0xF] = 1;
                    else
                        chip_walo->reg_v[0xF] = 0;

                    chip_walo->reg_v[X] = result;
                    chip_walo->pc += 2;
                }
                break;

                // Set register Vx = Vx SHR 1
                case 0x6: {
                    chip_walo->reg_v[X] = chip_walo->reg_v[Y];
                    uint8_t result = (chip_walo->reg_v[X] & 0x1);
                    chip_walo->reg_v[X] >>= 1;
                    chip_walo->reg_v[0xF] = result;
                    chip_walo->pc += 2;
                }
                break;

                // Set register Vx = Vy - Vx, Set register Vf = NOT borrow
                case 0x7: {
                    uint8_t result = chip_walo->reg_v[Y] - chip_walo->reg_v[X];
                    if (chip_walo->reg_v[Y] >= chip_walo->reg_v[X])
                        chip_walo->reg_v[0xF] = 1;
                    else
                        chip_walo->reg_v[0xF] = 0;

                    chip_walo->reg_v[X] = result;
                    chip_walo->pc += 2;
                } 
                break;

                // Set register Vx = Vx SHL 1
                case 0xE: {
                    chip_walo->reg_v[X] = chip_walo->reg_v[Y];
                    chip_walo->reg_v[X] <<= 1;
                    chip_walo->reg_v[0xF] = ((chip_walo->reg_v[X] & (1 << 7)) >> 7);
                    chip_walo->pc += 2;
                }
                break;

                default:
                    printf("UNKNOWN OPCODE: 0x%x\n", chip_walo->opcode);
                break;
            }
        break;

        // Skip next instruction if register Vx != Vy
        case 0x9000:
            if (chip_walo->reg_v[X] != chip_walo->reg_v[Y])
                chip_walo->pc += 4;
            else
                chip_walo->pc += 2;
        break;

        // Set I = NNN
        case 0xA000:
            chip_walo->i = ADDR;
            chip_walo->pc += 2;
        break;

        // Jump to location NNN + register Vx
        case 0xB000: {
            uint8_t XNN = ADDR & 0x0F00;
            chip_walo->pc = (chip_walo->reg_v[XNN] + ADDR);
        }
        break;

        // Set register Vx = random byte AND byte
        case 0xC000: {
            uint8_t random = (rand() % 0xFF);
            chip_walo->reg_v[X] = (random & BYTE);
            chip_walo->pc += 2;
        }
        break;
        
        // Display n - byte sprite starting at memory location I at(Vx, Vy), set VF = collision
        case 0xD000: {
            uint8_t x = chip_walo->reg_v[X] % SCREEN_WIDTH;
            uint8_t y = chip_walo->reg_v[Y] % SCREEN_HEIGHT;
            uint8_t height = NIBBLE;
            uint8_t px = 0;
            chip_walo->reg_v[0xF] = 0;

            for (uint8_t yline = 0; yline < height; yline++) {
                if ((y + yline) >= SCREEN_HEIGHT) break;
                px = chip_walo->mem[chip_walo->i + yline];
                for (uint8_t xline = 0; xline < 8; xline++) {
                    if ((x + xline) >= SCREEN_WIDTH) break;

                    if ((px & (0x80 >> xline))) {
                        if (chip_walo->gfx[x + xline + (y + yline) * SCREEN_WIDTH]) {
                            chip_walo->reg_v[0xF] = 1; // collision
                        }
                        chip_walo->gfx[x + xline + (y + yline) * SCREEN_WIDTH] ^= 1;
                    }
                }
            }
            
            chip_walo->draw_flag = 1;
            chip_walo->pc += 2;
        }
        break;

        case 0xE000:
            switch (BYTE) {
                // Skip next instruction if key with the value of register Vx is pressed
                case 0x9E:
                    if (chip_walo->key[chip_walo->reg_v[X]] == 1)
                        chip_walo->pc += 4;
                    else
                        chip_walo->pc += 2;
                break;

                // Skip next instruction if key with the value of Vx is not pressed 
                case 0xA1:
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
            switch (BYTE) {
                // Set register Vx = delay timer value
                case 0x07:
                    chip_walo->reg_v[X] = chip_walo->dt;
                    chip_walo->pc += 2;
                break;  
                
                // Wait for a key press, store the value of the key in register Vx 
                case 0x0A:
                    for (uint8_t key = 0; key < 16; key++) {
                        if (chip_walo->key[key] == 1) {
                            chip_walo->reg_v[X] = key;
                            chip_walo->pc += 2;
                            break;
                        }
                    }
                break;

                // Set delay timer = register Vx
                case 0x15:
                    chip_walo->dt = chip_walo->reg_v[X];
                    chip_walo->pc += 2;
                break;

                // Set sound timer = Vx
                case 0x18:
                    chip_walo->st = chip_walo->reg_v[X];
                    chip_walo->pc += 2;
                break;
                
                // Set I = I + register Vx
                case 0x1E:
                    chip_walo->i += chip_walo->reg_v[X];
                    chip_walo->pc += 2;
                break;

                // Set I = location of sprite for digit register Vx
                case 0x29:
                    chip_walo->i = (chip_walo->reg_v[X] * 5);
                    chip_walo->pc += 2;
                break;
  
                // Store BCD representation of register Vx in memory locations I,I + 1, and I + 2
                case 0x33:
                    chip_walo->mem[chip_walo->i] = (chip_walo->reg_v[X] / 100);
                    chip_walo->mem[chip_walo->i + 1] = (chip_walo->reg_v[X] / 10) % 10;
                    chip_walo->mem[chip_walo->i + 2] = (chip_walo->reg_v[X] % 100) % 10;
                    chip_walo->pc += 2;
                break;

                // Store registers V0 through Vx in memory starting at location I
                case 0x55:
                    for (uint32_t v = 0; v <= X; v++)
                        chip_walo->mem[chip_walo->i + v] = chip_walo->reg_v[v];
                        chip_walo->i++;

                    chip_walo->pc += 2;
                break;

                // Read registers V0 through Vx from memory starting at location I
                case 0x65:
                    for (uint32_t v = 0; v <= X; v++)
                        chip_walo->reg_v[v] = chip_walo->mem[chip_walo->i + v];
                        chip_walo->i++;

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

    if (chip_walo->st > 0) {
        chip_walo->st--;
        chip_walo->audio_flag = 1;
    }
}