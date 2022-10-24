#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "graphics.h"
#include "audio.h"
#include "keypad.h"

int main(int argc, char *argv[]) {
    // Load Rom
    if (argc == 2) {
        chip_walo = (Chip_walo *)malloc(sizeof(*chip_walo));
    } else {
        printf("Usage: xmake run chip-walo \"PATH/OF/THE/ROM\"\n");
        exit(SUCCESS);
    }

    core_init(chip_walo);
    core_load(chip_walo, argv[1]);
    gfx_init();
    audio_init();

    // cpu operation loop
    while (1) {
        key_detect(chip_walo);
        core_cycle(chip_walo);

        if (chip_walo->draw_flag == SET_DRAW) {
            gfx_update(chip_walo);
            chip_walo->draw_flag = 0;
        }

        if (chip_walo->audio_flag == SET_AUDIO) {
            audio_beep();
            chip_walo->audio_flag = 0;
        }
        SDL_Delay(3);
    }

    gfx_term();
    audio_term();
    free(chip_walo);
    chip_walo = NULL;
    return 0;
}