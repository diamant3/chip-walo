#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "audio.h"

SDL_AudioSpec *want = NULL;
SDL_AudioDeviceID audioDevice;
uint32_t sampleNR;


// source: https://stackoverflow.com/a/45002609
inline static void audio_callback(void *user_data, uint16_t *raw_buffer, uint32_t bytes) {
	uint32_t *buffer = raw_buffer;
	uint32_t length = bytes / 2;
	sampleNR = *(uint32_t *)user_data;

	for (uint32_t data = 0; data < length; ++data, ++sampleNR) {
		double_t time = (double_t)sampleNR / (double_t)AUDIO_SAMPLING_RATE;
		buffer[data] = (uint32_t)(AUDIO_AMPLITUDE * sin(2.0 * M_PI * 441.0 * time));
	}
}

void audio_init(void) {
	want = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));
	want->freq = AUDIO_SAMPLING_RATE;
	want->format = AUDIO_S8;
	want->channels = 1;
	want->samples = 2048;
	want->callback = audio_callback;
	want->userdata = &sampleNR;

	audioDevice = SDL_OpenAudioDevice(NULL, 0, want, NULL, 0);
	if (audioDevice <= 0) {
		printf("Opening audio failed, Error: %s\n", SDL_GetError());
		return;
	}
}

void audio_beep(void) {
	SDL_PauseAudioDevice(audioDevice, 0);
	SDL_Delay(64);
	SDL_PauseAudioDevice(audioDevice, 1);
}

void audio_term(void) {
	SDL_CloseAudioDevice(audioDevice);
	free(want);
}