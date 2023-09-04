#include <SDL2/SDL.h>

#include <math.h>
#ifndef M_PI
    # define M_PI 3.14159265358979323846
#endif

#include "chip_walo.h"
#include "peripherals.h"

static SDL_Window *win = NULL;
static SDL_Renderer *rdr = NULL;
static SDL_Texture *txr = NULL;

SDL_AudioSpec want;
static SDL_AudioDeviceID dev;
unsigned int sampleNR = 0;

static const SDL_KeyCode key_map[KEY_SZ] =
{
    SDLK_x, SDLK_1, SDLK_2,
    SDLK_3, SDLK_q, SDLK_w,
    SDLK_e, SDLK_a, SDLK_s,
    SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f,
    SDLK_v
};

char is_running = 0;

void CW_gfx_init(void)
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("[SDL INIT STATUS] %s\n", SDL_GetError());
        return;
    }

    win = SDL_CreateWindow(
        "CHIP-WALO",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (SCR_W * SCALE),
        (SCR_H * SCALE),
        SDL_WINDOW_SHOWN);

    if (win == NULL) 
    {
        printf("[WINDOW STATUS] %s\n", SDL_GetError());
        return;
    }

    rdr = SDL_CreateRenderer(
        win, 
        -1, 
        0);

    if (rdr == NULL) 
    {
        printf("[RENDERER STATUS] %s\n", SDL_GetError());
        return;
    }

    txr = SDL_CreateTexture(
        rdr,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCR_W,
        SCR_H);
    
    if (txr == NULL) 
    {
        printf("[TEXTURE STATUS] %s\n", SDL_GetError());
        return;
    }

    printf("[STATE] Graphics Initialized.\n");
}

void CW_gfx_out(void)
{
    static int px_buf[SCR_SZ] = { 0 };
    static const SDL_Rect fill = { 0, 0, (SCR_W * SCALE), (SCR_H * SCALE) };
    static const unsigned char R = BG_COLOR >> 24;
    static const unsigned char G = (BG_COLOR >> 16) & 0xff;
    static const unsigned char B = (BG_COLOR >> 8) & 0xff;
    static const unsigned char A = BG_COLOR & 0xff;

    for (int px = 0; px < SCR_SZ; px++)
    {
        px_buf[px] = ((FG_COLOR * chip_walo->gfx[px]) | BG_COLOR);
    }

    SDL_UpdateTexture(txr, NULL, px_buf, SCR_W * sizeof(*px_buf));
    SDL_SetRenderDrawColor(rdr, R, G, B, A);
    SDL_RenderFillRect(rdr, &fill);
    SDL_RenderCopy(rdr, txr, NULL, NULL);
    SDL_RenderPresent(rdr);
}

void CW_gfx_deinit(void)
{
    SDL_DestroyTexture(txr);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(win);
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    SDL_Quit();
    printf("[STATE] Graphics Deinitialized.\n");
}

static void audio_cb(void *userdata, unsigned short *raw_buf, int bytes)
{
    unsigned int len = (unsigned int)(bytes / 2);
    unsigned long long start_sample = (unsigned long long)userdata;
    double sample_timestep = 1.0f / AUDIO_SAMPLING_RATE;
    unsigned long long current_time = start_sample / AUDIO_SAMPLING_RATE;

    for (unsigned int w = 0; w < len; w++)
    {
        raw_buf[w] = (unsigned short)(AUDIO_AMPLITUDE * sin(2.0f * M_PI * 441.0f * current_time));
        current_time += (unsigned long long)sample_timestep;
    }

    *(unsigned int *)userdata += len;
}

void CW_audio_init(void)
{
    want.freq = AUDIO_SAMPLING_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = (SDL_AudioCallback)audio_cb;
    want.userdata = &sampleNR;

    dev = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
    if (dev == 0)
    {
        printf("[STATUS] %s\n", SDL_GetError());
        return;
    }

    printf("[STATE] Audio Initialized.\n");
}

void CW_audio_beep(void) 
{
    SDL_PauseAudioDevice(dev, 0);
    SDL_Delay(48);
    SDL_PauseAudioDevice(dev, 1);
}

void CW_audio_deinit(void) 
{
    SDL_CloseAudioDevice(dev);
    printf("[STATE] Audio Deinitialized.\n");
}

void CW_input_detect(void) 
{
    SDL_PumpEvents();
    const unsigned char *keyStates = SDL_GetKeyboardState(NULL);

    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_QUIT:
                is_running = 0;
                break;
        }
    }

    if (keyStates[SDL_SCANCODE_ESCAPE] || keyStates[SDL_SCANCODE_AC_BACK])
    {
        is_running = 0;
    }

    for (unsigned char key = 0; key < KEY_SZ; key++) 
    {
        SDL_Keycode mapped_key = key_map[key];
        if (keyStates[SDL_GetScancodeFromKey(mapped_key)]) { chip_walo->key[key] = 1; }
        else { chip_walo->key[key] = 0; }
    }
}