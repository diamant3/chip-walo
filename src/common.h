#ifndef COMMON_H
#define COMMON_H

// use exact datatypes
#include <stdint.h>

// use clear boolean values
#include <stdbool.h>

// graphic constants
#define SCREEN_SCALE  8    /* scaling factor for emulated pixel size */
#define SCREEN_WIDTH  64   /* width of the display */
#define SCREEN_HEIGHT 32   /* height of the display */
#define SCREEN_SIZE   2048 /* size of displayed graphics in pixel (64 x 32) */

// audio constants
#define AUDIO_SAMPLING_RATE 44100 /* sampling frequency of the audio */
#define AUDIO_AMPLITUDE     28000 /* amplitude/loudness of the audio */

// opcode shorthands
#define INSTR_POS_ADDR   (chip8.register_opcode & 0x0FFF) /* A 12-bit value, the lowest 12 bits of the instruction */
#define INSTR_POS_BYTE   (chip8.register_opcode & 0x00FF) /* An 8-bit value, the lowest 8 bits of the instruction */
#define INSTR_POS_NIBBLE (chip8.register_opcode & 0x000F) /* A 4-bit value, the lowest 4 bits of the instruction */
#define INSTR_POS_X      ((chip8.register_opcode & 0x0F00) >> 8) /* A 4-bit value, the lower 4 bits of the high byte of the instruction */
#define INSTR_POS_Y      ((chip8.register_opcode & 0x00F0) >> 4) /* A 4-bit value, the upper 4 bits of the low byte of the instruction */

// system constants
#define SYSTEM_FONT_SIZE 80 /* size of built-in font set */

// memory constants
#define STACK_SIZE           16   /* size of stack register */
#define MEMORY_SIZE          4096 /* size of the memory */
#define MEMORY_ADDRESS_START 512  /* start address of common chip8 rom load */

// register constants
#define REGISTER_SIZE 16

// input constants
#define KEYPAD_LENGTH 16 /* number of keys in keypad */

// chip-walo system struct
typedef struct chip_walo
{
    uint8_t register_v[REGISTER_SIZE];
    uint8_t system_memory[MEMORY_SIZE];
    uint8_t system_graphic[SCREEN_SIZE];
    uint8_t system_keypad[KEYPAD_LENGTH];
    uint8_t system_delay_timer;
    uint8_t system_sound_timer;
    uint8_t register_draw_flag;
    uint8_t register_sound_flag;
    uint16_t register_index;
    uint16_t register_program_counter;
    uint16_t register_opcode;
    uint16_t register_stack[STACK_SIZE];
    uint16_t register_stack_pointer;
} chip_walo_st;

extern chip_walo_st chip8;
extern uint8_t quit;

// graphics functions

void cw_peripheral_graphic_create(void);
void cw_peripheral_graphic_draw_pixel(void);
void cw_peripheral_graphic_destroy(void);

// key detect function

void cw_peripheral_key_detect(void);

// audio functions

void cw_peripheral_audio_create(void);
void cw_peripheral_audio_beep(void);
void cw_peripheral_audio_destroy(void);

// system/cpu functions

void cw_system_cpu_file_load(const char *rom);
void cw_system_cpu_cycle(void);
void cw_system_cpu_create(void);
void cw_system_cpu_destroy(void);
#endif