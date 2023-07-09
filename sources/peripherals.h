#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#define FG_COLOR 0x1976d2ff
#define BG_COLOR 0x212121ff
#define SCALE 8
#define AUDIO_SAMPLING_RATE 44100
#define AUDIO_AMPLITUDE 10000

void CW_gfx_init(void);
void CW_gfx_out(void);
void CW_gfx_deinit(void);
void CW_audio_init(void);
void CW_audio_deinit(void);
void CW_audio_beep(void);
void CW_input_detect(void);


extern char is_running;

#endif