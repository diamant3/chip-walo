#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#ifndef  M_PI
#define  M_PI  3.1415926535897932384626433
#endif

#include "common.h"

SDL_AudioSpec *want = NULL;
SDL_AudioDeviceID audioDevice;
uint32_t sampleNR;

static inline void cw_peripheral_audio_callback(void *userData, uint16_t *rawBuffer, uint32_t bytes) {
    uint32_t *buffer = rawBuffer;
    uint32_t length = bytes / 2;
    sampleNR = *(uint32_t *)userData;

    for (int32_t data = 0; data < length; ++data, ++sampleNR)
    {
        double_t time = (double_t)sampleNR / (double_t)AUDIO_SAMPLING_RATE;
        buffer[data] = (uint32_t)(AUDIO_AMPLITUDE * sin(2.0 * M_PI * 441.0 * time));
    }
}

void cw_peripheral_audio_create(void)
{
    want = (SDL_AudioSpec *) malloc(sizeof *want);
    want->freq = AUDIO_SAMPLING_RATE;
    want->format = AUDIO_S8;
    want->channels = 1;
    want->samples = 2048;
    want->callback = cw_peripheral_audio_callback;
    want->userdata = &sampleNR;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, want, NULL, 0);
    if (audioDevice <= 0) {
        printf("Opening audio failed, Error: %s\n", SDL_GetError());
    }
}

void cw_peripheral_audio_beep(void)
{
    SDL_PauseAudioDevice(audioDevice, 0);
    SDL_Delay(64);
    SDL_PauseAudioDevice(audioDevice, 1);
}

void cw_peripheral_audio_destroy(void)
{
    free(want);
    SDL_CloseAudioDevice(audioDevice);
}
