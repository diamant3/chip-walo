#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#define SCALE         8
#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32
#define SAMPLE_RATE 44100.0
#define AMPLITUDE   28000

void create_peripherals();
void draw_pixel();
void destroy_peripherals();
void detect_key();
void audio_callBack(void *userData, uint8_t *rawBuffer, int32_t bytes);
void beep();

#endif