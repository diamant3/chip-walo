#ifndef _AUDIO_H
#define _AUDIO_H

#ifndef M_PI
#define M_PI 3.1415926535897932384626433
#endif

// audio constants
#define AUDIO_SAMPLING_RATE 44100 /* sampling frequency of the audio */
#define AUDIO_AMPLITUDE 28000 /* amplitude/loudness of the audio */

// audio functions
void audio_init(void);
void audio_beep(void);
void audio_term(void);
#endif