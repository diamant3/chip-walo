#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "system.h"
#include "peripherals.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Rect rect;
SDL_Event event;
SDL_AudioSpec *want = NULL;
SDL_AudioDeviceID audioDevice;
uint8_t quit;
uint16_t sampleNR;

SDL_Scancode keymap[KEY_LENGTH] = {
  SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, // 1 2 3 4
  SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, // q w e r
  SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, // a s d f
  SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V  // z x c v
};

void audio_callBack(void *userData, uint8_t *rawBuffer, int32_t bytes) {
    uint16_t* buffer = (uint16_t*)rawBuffer;
    uint16_t length = bytes / 2;
    int32_t sampleNR = (*(int32_t*) userData);

    for (uint16_t data = 0; data < length; data++, sampleNR++) {
        double_t time = (double_t)sampleNR / (double_t)SAMPLE_RATE;
        buffer[data] = (uint16_t)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time));
    }
}

void beep() {
    SDL_PauseAudioDevice(audioDevice, 0);
    SDL_Delay(48);
    SDL_PauseAudioDevice(audioDevice, 1);
}

void draw_pixel() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (uint16_t ycor = 0; ycor < SCREEN_HEIGHT; ++ycor) {
        for (uint16_t xcor = 0; xcor < SCREEN_WIDTH; ++xcor) {
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

void detect_key() {
    if (SDL_PollEvent(&event)) {
        const uint8_t *state = SDL_GetKeyboardState(NULL);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
            break;
            default:
                for (uint8_t key = 0; key < KEY_LENGTH; ++key) {
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

void create_peripherals() {
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

void destroy_peripherals() {
    SDL_CloseAudioDevice(audioDevice);
    free(want);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); 
    SDL_Quit();
}