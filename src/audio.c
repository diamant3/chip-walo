#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "system.h"
#include "audio.h"

SDL_AudioSpec *want = NULL;
SDL_AudioDeviceID audioDevice;
unsigned int sampleNR = 0;

// source: https://stackoverflow.com/a/45002609
static void audio_callback(void *user_data, unsigned int *raw_buffer, unsigned int bytes) 
{
    unsigned int *buffer = raw_buffer;
    unsigned int length = (unsigned int)(bytes / 2);
    sampleNR = *(unsigned int *)user_data;

    for (unsigned int data = 0; data < length; ++data, ++sampleNR) 
    {
        double_t time = (double_t)sampleNR / (double_t)AUDIO_SAMPLING_RATE;
        buffer[data] = (unsigned int)(AUDIO_AMPLITUDE * sin(2.0 * M_PI * 441.0 * time));
    }
}

void audio_init(void) 
{
    want = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
    want->freq = AUDIO_SAMPLING_RATE;
    want->format = AUDIO_S8;
    want->channels = 1;
    want->samples = 2048;
    want->callback = audio_callback;
    want->userdata = &sampleNR;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, want, NULL, 0);
    if (audioDevice <= 0) 
    {
        printf("[FAIL] %s \n", SDL_GetError());
        exit(0);
    }
    printf("[SUCCESS] chip-walo Audio Initialized.\n");
}

void audio_beep(void) 
{
    SDL_PauseAudioDevice(audioDevice, 0);
    SDL_Delay(64);
    SDL_PauseAudioDevice(audioDevice, 1);
}

void audio_deinit(void) 
{
    SDL_CloseAudioDevice(audioDevice);
    free(want);
    printf("[SUCCESS] chip-walo Audio Deinitialized.\n");
}