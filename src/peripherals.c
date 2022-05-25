#define _GNU_SOURCE

#include "system.h"
#include "peripherals.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Rect rect;
SDL_Event event;
SDL_AudioSpec *want = NULL;
SDL_AudioDeviceID audioDevice;
unsigned char quit;
unsigned short sampleNR;

SDL_Scancode keymap[KEY_LENGTH] = {
  SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, // 1 2 3 4
  SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, // q w e r
  SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, // a s d f
  SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V  // z x c v
};

static inline void audio_callBack(void *userData, unsigned char *rawBuffer, int bytes) {
    unsigned short* buffer = (unsigned short*)rawBuffer;
    unsigned short length = bytes / 2;
    int sampleNR = (*(int*) userData);

    for (unsigned short data = 0; data < length; data++, sampleNR++) {
        double time = (double)sampleNR / (double)SAMPLE_RATE;
        buffer[data] = (unsigned short)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time));
    }
}

/**
    @brief     Produce beep sound
    @return    Nothing
*/
void beep(void) {
    SDL_PauseAudioDevice(audioDevice, 0);
    SDL_Delay(50);
    SDL_PauseAudioDevice(audioDevice, 1);
}

/**
    @brief     Draw a pixel
    @return    Nothing
*/
void draw_pixel(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int ycor = 0; ycor < SCREEN_HEIGHT; ++ycor) {
        for (int xcor = 0; xcor < SCREEN_WIDTH; ++xcor) {
            if (chip8.gfx[xcor + (ycor * SCREEN_WIDTH)]) {
                rect.x = (xcor * SCALE);
                rect.y = (ycor * SCALE);
                rect.w = SCALE;
                rect.h = SCALE;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

/**
    @brief     Get an input from keyboard
    @return    Nothing
*/
void detect_key(void) {
    if (SDL_PollEvent(&event)) {
        const unsigned char *state = SDL_GetKeyboardState(NULL);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
            break;
            default:
                for (int key = 0; key < KEY_LENGTH; ++key) {
                    chip8.keypad[key] = state[keymap[key]];
                    if (state[SDL_SCANCODE_ESCAPE]) {
                        quit = 1;
                    }
                }
            break;
        }
    }
    SDL_Delay(2);
}

/**
    @brief     Create GUI/Sound
    @return    Nothing
*/
void create_peripherals(void) {
    want = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
    want->freq = SAMPLE_RATE;
    want->format = AUDIO_S8;
    want->channels = 1;
    want->samples = 2048;
    want->callback = audio_callBack;
    want->userdata = &sampleNR;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL Init failed: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(
        "chip-walo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (SCREEN_WIDTH * SCALE),
        (SCREEN_HEIGHT * SCALE),
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (window == NULL || renderer == NULL) {
        printf("Window creation failed: %s\n", SDL_GetError());
    }

    audioDevice = SDL_OpenAudioDevice(NULL, 0, want, NULL, 0);
    if (audioDevice < 1) {
        printf("Failed to open audio: %s\n", SDL_GetError());
    }
}

/**
    @brief     Destroy GUI/Sound
    @return    Nothing
*/
void destroy_peripherals(void) {
    free(want);
    SDL_CloseAudioDevice(audioDevice);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); 
    SDL_Quit();
}