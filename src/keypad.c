#include <SDL2/SDL.h>

#include "common.h"

uint8_t quit = false;

// 16 keys corresponding to the keypad of chip-8
uint8_t key_map[KEYPAD_LENGTH] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

void cw_peripheral_key_detect(void)
{
    SDL_Event event;
    SDL_Delay(3);

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            quit = true;
        }

        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            }

            for (uint8_t key = 0; key < KEYPAD_LENGTH; ++key)
            {
                if (event.key.keysym.sym == key_map[key])
                {
                    chip8.system_keypad[key] = 0x1;
                }
            }
        }

        if (event.type == SDL_KEYUP)
        {
            for (uint8_t key = 0; key < KEYPAD_LENGTH; ++key)
            {
                if (event.key.keysym.sym == key_map[key])
                {
                    chip8.system_keypad[key] = 0x0;
                }
            }
        }
    }
}