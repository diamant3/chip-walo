#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#include "chip_walo.h"
#include "peripherals.h"

int check_ext(const char *filename)
{
    const char *ext = ".ch8";
    const char *find = strrchr(filename, '.');
    if (find != NULL && strcmp(find, ext) == 0) { return 1; }
    return 0;
}

int main(int argc, char *argv[]) 
{
    if (argc == 2) 
    {
        if (check_ext(argv[1])) 
        { 
            CW_init();
            CW_gfx_init();
            CW_audio_init();
            CW_loadrom(argv[1]);
            is_running = 1;
        }
        else { 
            printf("Please input .ch8 file.\n"); 
            return -1; 
        }
    } 
    else 
    {
        printf("Usage: xmake run chip-walo \"sample.ch8\"\n");
        return -1;
    }

    unsigned int prev_time = SDL_GetTicks();
    while (is_running) 
    {
        CW_input_detect();
        CW_cycle();

        if (chip_walo->draw_flag == 1) 
        {
            CW_gfx_out();
            chip_walo->draw_flag = 0;
        }

        if (chip_walo->audio_flag == 1) 
        {
            CW_audio_beep();
            chip_walo->audio_flag = 0;
        }

        unsigned int curr_time = SDL_GetTicks();
        unsigned int elapsed_time = curr_time - prev_time;
        if (elapsed_time < 4) { SDL_Delay(4 - elapsed_time); }
        prev_time = curr_time;
    }

    CW_gfx_deinit();
    CW_audio_deinit();
    CW_deinit();
    printf("[STATE] System Stopped.\n");
    return 0;
}
