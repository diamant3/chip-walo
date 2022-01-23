#include "../header/system.h"
#include "../header/peripherals.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// Initialize for window and renderer
void init_peripherals()
{
    int err = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if(err)
    {
        printf("SDL Init failed: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(
        "chip-walo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (SCREEN_WIDTH * SCALE),
        (SCREEN_HEIGHT * SCALE),
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(
        window,
        -1,
        0
    );

    if(window == NULL || renderer == NULL) {
        printf("Window/Renderer creation failed: %s\n", SDL_GetError());
    }
}

SDL_Rect rect;

// Draw a pixel
void draw_graphics()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int ycor, xcor;
    for (ycor = 0; ycor < SCREEN_HEIGHT; ycor++) {
        for (xcor = 0; xcor < SCREEN_WIDTH; xcor++) {
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

// Close the window & renderer
void close_peripherals()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); 
}

SDL_Scancode keymap[KEY_LENGTH] = {
  SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, // 1 2 3 4
  SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, // q w e r
  SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, // a s d f
  SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V  // z x c v
};

SDL_Event event;
int quit = 0;

// Key events for chip-8
void key_press()
{   
    if(SDL_PollEvent(&event))
    {
        const unsigned char *state = SDL_GetKeyboardState(NULL);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
            break;
            default:
                for (int key = 0; key < KEY_LENGTH; key++) {
                    chip8.keypad[key] = state[keymap[key]];
                }

                if(state[SDL_SCANCODE_ESCAPE]) {
                    quit = 1;
                }
            break;
        }
    }

}

SDL_AudioSpec *want;
SDL_AudioDeviceID audioDevice;
int sampleNR;

// callback for audio
void audio_callBack(void *userData, unsigned char *rawBuffer, int bytes)
{
    short *buffer = (short *)rawBuffer;
    int length = bytes / 2;
    int sampleNR = (*(int *) userData);

    for(int data = 0; data < length; data++, sampleNR++)
    {
        double time = (double)sampleNR / (double)SAMPLE_RATE;
        buffer[data] = (short)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time));
    }
}

// Initialize audio system
void init_sound()
{
    want = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));
    want->freq = SAMPLE_RATE;
    want->format = AUDIO_S8;
    want->channels = 1;
    want->samples = 2048;
    want->callback = audio_callBack;
    want->userdata = &sampleNR;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, want, NULL, 0);
    if(audioDevice == 0)
    {
        printf("Failed to open audio: %s\n", SDL_GetError());
    }
}

// De init the audio system
void close_audio()
{
    if(audioDevice != 0) {
        SDL_CloseAudioDevice(audioDevice);
        audioDevice = 0;
    }

    if(want != 0) {
        want = NULL;
        free(want);
    }

    return;
}

// beep function
void beep_sound()
{
    SDL_PauseAudioDevice(audioDevice, 0);
    SDL_Delay(48);
    SDL_PauseAudioDevice(audioDevice, 1);
}