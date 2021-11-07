#include "headers/system.h"
#include "headers/peripherals.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

extern Processor chip8;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Event event;
SDL_Rect rect;
SDL_AudioSpec *audioSpec = NULL;
SDL_AudioDeviceID audioDevice = 0;
int sampleNR;
int quit = 0;

SDL_Scancode keymap[KEY_LENGTH] = {
  SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, // 1 2 3 4
  SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, // q w e r
  SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, // a s d f
  SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V  // z x c v
};

// START GRAPHICS SECTION

void initPeripherals()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
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
}

void drawGraphics()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int ycor = 0; ycor < SCREEN_HEIGHT; ycor++) {
        for (int xcor = 0; xcor < SCREEN_WIDTH; xcor++) {
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


void closePeripherals()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); 
}

// END GRAPHICS SECTION

// START INPUT SECTION

void keyPress()
{   
    if(SDL_PollEvent(&event))
    {
        const unsigned char* state = SDL_GetKeyboardState(NULL);
        switch (event.type)
        {
            case SDL_QUIT:
                quit = 1;
            break;

            default:
                for (int key = 0; key < KEY_LENGTH; key++) {
                    chip8.keypad[key] = state[keymap[key]];
                }

                if(state[SDL_SCANCODE_ESCAPE])
                {
                    quit = 1;
                }
            break;
        }
    }
}

int exitMachine()
{
    return quit;
}

// END INPUT SECTION

// START AUDIO SECTION

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

void initSound()
{
    sampleNR = 0;

    audioSpec = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));
    audioSpec->freq = SAMPLE_RATE;
    audioSpec->format = AUDIO_S8;
    audioSpec->channels = 1;
    audioSpec->samples = 2048;
    audioSpec->callback = audio_callBack;
    audioSpec->userdata = &sampleNR;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, audioSpec, NULL, 0);
    if(audioDevice != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s\n", SDL_GetError());
    }
}

void closeAudio()
{
    if(audioDevice != 0)
    {
        SDL_CloseAudioDevice(audioDevice);
        audioDevice = 0;
    }

    if(audioSpec != 0)
    {
        free(audioSpec);
        audioSpec = NULL;
    }
}

void beepSound()
{
    SDL_PauseAudioDevice(audioDevice, 0);
    SDL_Delay(40);
    SDL_PauseAudioDevice(audioDevice, 1);
}

// END AUDIO SECTION