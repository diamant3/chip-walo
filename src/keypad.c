#include <SDL2/SDL.h>
#include <stdio.h>

#include "common.h"

SDL_Event event;
uint8_t quit = false;

// 16 keys corresponding to the keypad of chip-8 
SDL_Scancode key_map[KEYPAD_LENGTH] = {
  SDL_SCANCODE_1, 
  SDL_SCANCODE_2, 
  SDL_SCANCODE_3, 
  SDL_SCANCODE_4,
  SDL_SCANCODE_Q, 
  SDL_SCANCODE_W, 
  SDL_SCANCODE_E, 
  SDL_SCANCODE_R,
  SDL_SCANCODE_A, 
  SDL_SCANCODE_S, 
  SDL_SCANCODE_D, 
  SDL_SCANCODE_F,
  SDL_SCANCODE_Z, 
  SDL_SCANCODE_X, 
  SDL_SCANCODE_C, 
  SDL_SCANCODE_V
};

void cw_peripheral_key_detect(void)
{
    SDL_Delay(2);
    if (SDL_PollEvent(&event)) 
    {
        const uint8_t *state = SDL_GetKeyboardState(NULL);
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
            break;

            default:
                for (uint32_t key = 0; key < KEYPAD_LENGTH; ++key) 
                {
                    chip8.system_keypad[key] = state[key_map[key]];
                    if (state[SDL_SCANCODE_ESCAPE]) 
                    {
                        quit = true;
                    }
                }
            break;
        }
    } 
}