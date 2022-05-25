#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#define SCALE         8
#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32
#define SAMPLE_RATE   44100
#define AMPLITUDE     28000

void create_peripherals(void);
void draw_pixel(void);
void destroy_peripherals(void);
void detect_key(void);
void beep(void);

#endif