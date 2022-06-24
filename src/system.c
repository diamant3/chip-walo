#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "common.h"

chip_walo_st chip8;

uint8_t font_set[SYSTEM_FONT_SIZE] = {
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

void cw_system_cpu_destroy(void)
{
    chip8.register_index = 0;
    chip8.register_opcode = 0;
    chip8.register_stack_pointer = 0;
    chip8.register_program_counter = 0;
}

void cw_system_cpu_create(void)
{
    srand((uint32_t)time(NULL));
    chip8.register_index = 0;
    chip8.register_opcode = 0;
    chip8.register_stack_pointer = 0;
    chip8.register_program_counter = MEMORY_ADDRESS_START;

    for (uint32_t font = 0; font < SYSTEM_FONT_SIZE; ++font)
    {
        chip8.system_memory[font] = font_set[font];
    }
}

void cw_system_cpu_file_load(const uint8_t *rom) {
    uint64_t rom_size;
    uint8_t *rom_buffer = NULL;
    FILE *file = NULL;

    // open the rom file
    file = fopen(rom, "rb");
    if (file != NULL)
    {
        // Get the rom size & buffer
        fseek(file, 0, SEEK_END);
        rom_size = ftell(file);
        printf("ROM Size: %d\n", rom_size);
        rewind(file);
        rom_buffer = (uint8_t *) malloc(rom_size * (sizeof rom_buffer));
    }
    else
    {
        fprintf(stderr, "Opening file failed, Error: %s\n", strerror(errno));
    }

    // Check and Load the rom to memory
    uint32_t status = fread(rom_buffer, 1, rom_size, file);
    if (status > 0)
    {
        if (rom_size < MEMORY_SIZE)
        {
            for (uint32_t data = 0; data < rom_size; ++data)
            {
                chip8.system_memory[MEMORY_ADDRESS_START + data] = rom_buffer[data];
            }
        }
        else
        {
            fprintf(stderr, "Loading rom failed, Error: %s\n", strerror(errno));
        }
    }
    else
    {
        fprintf(stderr, "Loading rom failed, Error: %s\n", strerror(errno));
    }

    free(rom_buffer);
    fclose(file);
}

void cw_system_cpu_cycle(void)
{
    chip8.register_opcode = chip8.system_memory[chip8.register_program_counter] << 8 \
    | chip8.system_memory[chip8.register_program_counter + 1];

    switch (chip8.register_opcode & 0xF000)
    {
        case 0x0000:
            switch (INSTR_POS_BYTE)
            {
                // Clear the display
                case 0xE0:
                    memset(chip8.system_graphic, 0, SCREEN_SIZE);
                    chip8.register_program_counter += 2;
                break;

                // Return from a subroutine
                case 0xEE:
                    --chip8.register_stack_pointer;
                    chip8.register_program_counter = chip8.register_stack[chip8.register_stack_pointer];
                    chip8.register_program_counter += 2;
                break;

                default:
                    printf("Unknown op code: %x\n", chip8.register_opcode);
                break;
            }
            break;
        break;

        // Jump to location NNN
        case 0x1000:
            chip8.register_program_counter = INSTR_POS_ADDR;
        break;

        // Call subroutine at NNN
        case 0x2000:
            chip8.register_stack[chip8.register_stack_pointer] = chip8.register_program_counter;
            ++chip8.register_stack_pointer;
            chip8.register_program_counter = INSTR_POS_ADDR;
        break;

        // Skip next instruction if register Vx == byte
        case 0x3000:
            if (chip8.register_v[INSTR_POS_X] == INSTR_POS_BYTE)
            {
                chip8.register_program_counter += 4;
            }
            else
            {
                chip8.register_program_counter += 2;
            }
        break;

        // Skip next instruction if register Vx != byte
        case 0x4000:
            if (chip8.register_v[INSTR_POS_X] != INSTR_POS_BYTE)
            {
                chip8.register_program_counter += 4;
            }
            else
            {
                chip8.register_program_counter += 2;
            }
        break;

        // Skip next instruction if register Vx == register Vy
        case 0x5000:
            if (chip8.register_v[INSTR_POS_X] == chip8.register_v[INSTR_POS_Y])
            {
                chip8.register_program_counter += 4;
            }
            else
            {
                chip8.register_program_counter += 2;
            }
        break;

        // Set register Vx = byte
        case 0x6000:
            chip8.register_v[INSTR_POS_X] = INSTR_POS_BYTE;
            chip8.register_program_counter += 2;
        break;

       // Set register Vx = Vx + byte
        case 0x7000:
            chip8.register_v[INSTR_POS_X] += INSTR_POS_BYTE;
            chip8.register_program_counter += 2;
        break;

        case 0x8000:
            switch (INSTR_POS_NIBBLE)
            {
                // Set register Vx = Vy
                case 0x0:
                    chip8.register_v[INSTR_POS_X] = chip8.register_v[INSTR_POS_Y];
                    chip8.register_program_counter += 2;
                break;

                // Set register Vx = Vx OR Vy
                case 0x1:
                    chip8.register_v[INSTR_POS_X] |= chip8.register_v[INSTR_POS_Y];
                    chip8.register_program_counter += 2;
                break;

                // Set register Vx = Vx AND Vy
                case 0x2:
                    chip8.register_v[INSTR_POS_X] &= chip8.register_v[INSTR_POS_Y];
                    chip8.register_program_counter += 2;
                break;

                // Set register Vx = Vx XOR Vy
                case 0x3:
                    chip8.register_v[INSTR_POS_X] ^= chip8.register_v[INSTR_POS_Y];
                    chip8.register_program_counter += 2;
                break;

                // Set register Vx = Vx + Vy, Set register Vf = 1 if overflow
                case 0x4:
                    {
                        chip8.register_v[0xF] = false;
                        uint32_t result = chip8.register_v[INSTR_POS_X] + chip8.register_v[INSTR_POS_Y];
                        uint8_t overflow = false;

                        if (result > 0xFF)
                        {
                            overflow = true;
                        }
                        else
                        {
                            overflow = false;
                        }
                        chip8.register_v[INSTR_POS_X] = result & 0xFF;
                        chip8.register_v[0xF] = overflow;
                        chip8.register_program_counter += 2;
                    }
                break;

                // Set register Vx = Vx - Vy, Set register Vf = NOT borrow
                case 0x5:
                    {
                        chip8.register_v[0xF] = false;
                        uint8_t overflow = false;

                        if (chip8.register_v[INSTR_POS_X] >= chip8.register_v[INSTR_POS_Y]) {
                            overflow = true;
                        } else {
                            overflow = false;
                        }
		    	        chip8.register_v[INSTR_POS_X] = chip8.register_v[INSTR_POS_X] - chip8.register_v[INSTR_POS_Y];
			            chip8.register_v[0xF] = overflow;
                        chip8.register_program_counter += 2;
                    }
                break;

                // Set register Vx = Vx SHR 1
                case 0x6:
                    {
                        uint8_t overflow = chip8.register_v[INSTR_POS_X] & 1;
                        chip8.register_v[INSTR_POS_X] >>= 1;
                        chip8.register_v[0xF] = overflow;
                        chip8.register_program_counter += 2;
                    }
                break;

                // Set register Vx = Vy - Vx, Set register Vf = NOT borrow
                case 0x7:
                    {
                        chip8.register_v[0xF] = false;
                        uint8_t overflow = false;

                        if (chip8.register_v[INSTR_POS_Y] >= chip8.register_v[INSTR_POS_X]) {
                            overflow = true;
                        } else {
                            overflow = false;
                        }
			            chip8.register_v[INSTR_POS_X] = chip8.register_v[INSTR_POS_Y] - chip8.register_v[INSTR_POS_X];
			            chip8.register_v[0xF] = overflow;
                        chip8.register_program_counter += 2;
                    }
                break;

                // Set register Vx = Vx SHL 1
                case 0xE:
                {
                    uint8_t overflow = false;

                    if (chip8.register_v[INSTR_POS_X] >> 7)
                    {
                        overflow = 1;
                    }
                    else
                    {
                        overflow = false;
                    }
                    chip8.register_v[INSTR_POS_X] <<= 1;
                    chip8.register_v[0xF] = overflow;
                    chip8.register_program_counter += 2;
                }
                break;

                default:
                    printf("Unknown op code: %x\n", chip8.register_opcode);
                break;
            }
            break;

        // Skip next instruction if register Vx != Vy
        case 0x9000:
            if (chip8.register_v[INSTR_POS_X] != chip8.register_v[INSTR_POS_Y])
            {
                chip8.register_program_counter += 4;
            }
            else
            {
                chip8.register_program_counter += 2;
            }
        break;

        // Set I = NNN
        case 0xA000:
            chip8.register_index = INSTR_POS_ADDR;
            chip8.register_program_counter += 2;
        break;

        // Jump to location NNN + register V0
        case 0xB000:
            chip8.register_program_counter = chip8.register_v[0] + INSTR_POS_ADDR;
        break;

        // Set register Vx = random byte AND byte
        case 0xC000:
            chip8.register_v[INSTR_POS_X] = (rand() % 256) & INSTR_POS_BYTE;
            chip8.register_program_counter += 2;
        break;

        // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
        case 0xD000:
            {
                uint16_t x = chip8.register_v[INSTR_POS_X];
                uint16_t y = chip8.register_v[INSTR_POS_Y];
                uint16_t height = INSTR_POS_NIBBLE;
                uint16_t pixel;
                chip8.register_v[0xF] = false;

                for (uint32_t yline = 0; yline < height; ++yline)
                {
                    pixel = chip8.system_memory[chip8.register_index + yline];
                    for (uint32_t xline = 0; xline < 8; ++xline)
                    {
                        if ((pixel & (0x80 >> xline)) != 0)
                        {
                            if (chip8.system_graphic[((x + xline) + ((y + yline) * SCREEN_WIDTH))] == true)
                            {
                                chip8.register_v[0xF] = true;
                            }
                            chip8.system_graphic[((x + xline) + ((y + yline) * SCREEN_WIDTH))] ^= true;
                        }
                    }
                }
                chip8.register_draw_flag = true;
                chip8.register_program_counter += 2;
            }
        break;

        case 0xE000:
            switch(INSTR_POS_BYTE)
            {
                // Skip next instruction if key with the value of register Vx is pressed
                case 0x9E:
                    if (chip8.system_keypad[chip8.register_v[INSTR_POS_X]] == true)
                    {
                        chip8.register_program_counter += 4;
                    }
                    else
                    {
                        chip8.register_program_counter += 2;
                    }
                break;

                // Skip next instruction if key with the value of Vx is not pressed
                case 0xA1:
                    if (chip8.system_keypad[chip8.register_v[INSTR_POS_X]] == false)
                    {
                        chip8.register_program_counter += 4;
                    }
                    else
                    {
                        chip8.register_program_counter += 2;
                    }
                break;

                default:
                    printf("Unknown op code: %x\n", chip8.register_opcode);
                break;
            }
        break;

        case 0xF000:
            switch (INSTR_POS_BYTE)
            {
                // Set register Vx = delay timer value
                case 0x07:
                    chip8.register_v[INSTR_POS_X] = chip8.system_delay_timer;
                    chip8.register_program_counter += 2;
                break;

                // Wait for a key press, store the value of the key in register Vx
                case 0x0A:
                    for (uint32_t key = 0; key < KEYPAD_LENGTH; ++key)
                    {
                        if (chip8.system_keypad[key] == true)
                        {
                            chip8.register_v[INSTR_POS_X] = key;
                            chip8.register_program_counter += 2;
                            break;
                        }
                    }
                break;

                // Set delay timer = register Vx
                case 0x15:
                    chip8.system_delay_timer = chip8.register_v[INSTR_POS_X];
                    chip8.register_program_counter += 2;
                break;

                // Set sound timer = Vx
                case 0x18:
                    chip8.system_sound_timer = chip8.register_v[INSTR_POS_X];
                    chip8.register_program_counter += 2;
                break;

                // Set I = I + register Vx
                case 0x1E:
                    chip8.register_index += chip8.register_v[INSTR_POS_X];
                    chip8.register_program_counter += 2;
                break;

                // Set I = location of sprite for digit register Vx
                case 0x29:
                    chip8.register_index = (chip8.register_v[INSTR_POS_X] * 5);
                    chip8.register_program_counter += 2;
                break;

                // Store BCD representation of register Vx in memory locations I, I+1, and I+2
                case 0x33:
                    chip8.system_memory[chip8.register_index] = (chip8.register_v[INSTR_POS_X] / 100) % 10;
                    chip8.system_memory[chip8.register_index + 1] = (chip8.register_v[INSTR_POS_X] / 10) % 10;
                    chip8.system_memory[chip8.register_index + 2] = chip8.register_v[INSTR_POS_X] % 10;
                    chip8.register_program_counter += 2;
                break;

                // Store registers V0 through Vx in memory starting at location I
                case 0x55:
                    for (uint32_t vi = 0; vi <= INSTR_POS_X; ++vi)
                    {
                        chip8.system_memory[chip8.register_index + vi] = chip8.register_v[vi];
                    }
                    chip8.register_program_counter += 2;
                break;

                // Read registers V0 through Vx from memory starting at location I
                case 0x65:
                    for (uint32_t vi = 0; vi <= INSTR_POS_X; ++vi)
                    {
                        chip8.register_v[vi] = chip8.system_memory[chip8.register_index + vi];
                    }
                    chip8.register_program_counter += 2;
                break;

                default:
                    printf("Unknown op code: %x\n", chip8.register_opcode);
                break;
            }
        break;
    }

    // Update Timers
    if (chip8.system_delay_timer > 0)
    {
        --chip8.system_delay_timer;
    }

    if (chip8.system_sound_timer > 0)
    {
        --chip8.system_sound_timer;
        chip8.register_sound_flag = true;
    }
}