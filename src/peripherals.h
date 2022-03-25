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
    void init_graphics();
    void draw_graphics();
    void close_graphics();

    // input functions
    void key_press();

    // audio handlers
    void audio_callBack(void *userData, uint8_t *rawBuffer, int32_t bytes);
    void init_audio();
    void close_audio();
    void beep_audio();

#endif