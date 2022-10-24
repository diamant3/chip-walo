#include <SDL2/SDL.h>

#include <stdio.h>

#include "system.h"
#include "graphics.h"

/*
    Color channel format: A-R-G-B
    example: FF-3F-F5-B5
    tip: A is for opacity, change the RGB only
*/
#define FG_COLOR 0xFF1976D2
#define BG_COLOR 0xFF212121

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Texture * texture = NULL;

void gfx_init(void) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("[ERROR]: %s \n", SDL_GetError());
        exit(FAILURE);
    }

    window = SDL_CreateWindow(
        "chip-walo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (SCREEN_WIDTH * SCREEN_SCALE),
        (SCREEN_HEIGHT * SCREEN_SCALE),
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("[ERROR]: %s \n", SDL_GetError());
        exit(FAILURE);
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == NULL) {
        printf("[ERROR]: %s\n", SDL_GetError());
        exit(FAILURE);
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if (texture == NULL) {
        printf("[ERROR]: %s\n", SDL_GetError());
        exit(FAILURE);
    }
}

void gfx_update(Chip_walo *cw) {
    u32 px_buffer[SCREEN_SIZE] = {0};

    for (u32 px = 0; px < SCREEN_SIZE; ++px) {
        px_buffer[px] = ((FG_COLOR * cw->gfx[px]) | BG_COLOR);
    }

    SDL_UpdateTexture(texture, NULL, px_buffer, SCREEN_WIDTH * (sizeof(px_buffer[0])));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void gfx_term(void) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    SDL_Quit();
}