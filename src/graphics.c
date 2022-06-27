#include <SDL2/SDL.h>
#include <stdio.h>

#include "common.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Rect rect;

void cw_peripheral_graphic_create(void)
{
    if (SDL_InitSubSystem(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL initialization failed, Error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(
        "chip-walo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (SCREEN_WIDTH * SCREEN_SCALE),
        (SCREEN_HEIGHT * SCREEN_SCALE),
        0);
    if (window == NULL)
    {
        printf("Creating a window object failed, Error: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(
        renderer,
        SCREEN_WIDTH * SCREEN_SCALE,
        SCREEN_HEIGHT * SCREEN_SCALE);
    if (renderer == NULL)
    {
        printf("Creating a rendering context failed, Error: %s\n", SDL_GetError());
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    if (texture == NULL)
    {
        printf("Creating a texture failed, Error: %s\n", SDL_GetError());
    }
}

void cw_peripheral_graphic_draw_pixel(void)
{
    uint32_t pixels[SCREEN_SIZE];
    for (uint32_t px = 0; px < SCREEN_SIZE; ++px)
    {
        pixels[px] = (0x00FFFFFF * chip8.system_graphic[px]) | 0xFF000000;
    }

    SDL_UpdateTexture(
        texture,
        NULL,
        pixels,
        SCREEN_WIDTH * (sizeof pixels[0]));

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void cw_peripheral_graphic_destroy(void)
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    SDL_Quit();
}