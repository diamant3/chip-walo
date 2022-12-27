#include <SDL2/SDL.h>

#include <stdio.h>

#include "system.h"
#include "keypad.h"

// 16 keys corresponding to the keypad of chip-8
u8 key_map[KEY_COUNT] = 
{
    SDLK_x, SDLK_1, SDLK_2,
    SDLK_3, SDLK_q, SDLK_w,
    SDLK_e, SDLK_a, SDLK_s,
    SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f,
    SDLK_v
};

void key_detect(Chip_walo *cw) 
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) 
    {
        if (event.type == SDL_QUIT) 
            exit(0);

        if (event.type == SDL_KEYDOWN) 
        {
            if (event.key.keysym.sym == SDLK_ESCAPE) 
                exit(0);

            for (u8 key = 0; key < KEY_COUNT; ++key) 
            {
                if (event.key.keysym.sym == key_map[key])
                    cw -> key[key] = 1;
            }
        }

        if (event.type == SDL_KEYUP) 
        {
            for (u8 key = 0; key < KEY_COUNT; ++key) 
            {
                if (event.key.keysym.sym == key_map[key])
                    cw -> key[key] = 0;
            }
        }
    }
}