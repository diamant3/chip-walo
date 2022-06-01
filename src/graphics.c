#include <SDL2/SDL.h>
#include <stdio.h>

#include "common.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Rect rect;

void cw_peripheral_graphic_create(void) 
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) 
    {
        printf("SDL initialization failed, Error: %s\n", SDL_GetError());
    }   

    window = SDL_CreateWindow(
        "chip-walo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (SCREEN_WIDTH * SCREEN_SCALE),
        (SCREEN_HEIGHT * SCREEN_SCALE),
        SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) 
    {
        printf("Creating a window object failed, Error: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) 
    {
        printf("Creating a rendering context failed, Error: %s\n", SDL_GetError());
    }
}

void cw_peripheral_graphic_draw_pixel(void) 
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (uint32_t ycor = 0; ycor < SCREEN_HEIGHT; ++ycor) 
    {
        for (uint32_t xcor = 0; xcor < SCREEN_WIDTH; ++xcor) 
        {
            if (chip8.system_graphic[xcor + (ycor * SCREEN_WIDTH)]) 
            {
                rect.x = (xcor * SCREEN_SCALE);
                rect.y = (ycor * SCREEN_SCALE);
                rect.w = SCREEN_SCALE;
                rect.h = SCREEN_SCALE;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void cw_peripheral_graphic_destroy(void) 
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); 
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
    SDL_Quit();
}