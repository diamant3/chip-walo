#include <SDL2/SDL.h>

#include <stdbool.h>

#include "system.h"

#include "keypad.h"

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

void key_detect(Chip_walo * cw) {
  SDL_Event event;

  while (SDL_PollEvent( & event)) {
    if (event.type == SDL_QUIT) {
      quit = true;
    }

    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        quit = true;
      }

      for (size_t key = 0; key < KEYPAD_LENGTH; ++key) {
        if (event.key.keysym.sym == key_map[key]) {
          cw -> keypad[key] = true;
        }
      }
    }

    if (event.type == SDL_KEYUP) {
      for (size_t key = 0; key < KEYPAD_LENGTH; ++key) {
        if (event.key.keysym.sym == key_map[key]) {
          cw -> keypad[key] = false;
        }
      }
    }
  }
}