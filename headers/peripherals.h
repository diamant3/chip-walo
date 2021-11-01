#ifndef PERIPHERALS_H
#define PERIPHERALS_H

// Resolution
#define SCALE         8
#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32

// audio Constants
#define SAMPLE_RATE 44100.0
#define AMPLITUDE   28000

//  gfx Functions
void initPeripherals();
void drawGraphics();
void closePeripherals();

// input functions
void keyPress();
int exitMachine();

// audio handlers
void audio_callBack(void *userData, unsigned char *rawBuffer, int bytes);
void initSound();
void closeAudio();
void beepSound();
#endif