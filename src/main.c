#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "system.h"
#include "graphics.h"
#include "audio.h"
#include "keypad.h"

int32_t main(int32_t argc, int8_t *argv[]) {
    Chip_walo *chip_walo = NULL;
    chip_walo = (Chip_walo *) malloc(sizeof(Chip_walo));

    // Load Rom
    if (argc == 2) {
        core_init(chip_walo);
        core_load(chip_walo, argv[1]);
        gfx_init();
        audio_init();
    } else {
        printf("Usage: xmake run chip-walo \"PATH/OF/THE/ROM\"");
        quit = true;
    }

    // cpu operation loop
    while (!quit) {
        SDL_Delay(2);
        core_cycle(chip_walo);
        key_detect(chip_walo);

        if (chip_walo->draw_flag) {
            gfx_update(chip_walo);
            chip_walo->draw_flag = false;
        }

        if (chip_walo->audio_flag) {
            audio_beep();
            chip_walo->audio_flag = false;
        }

        if (quit) {
            gfx_term();
            audio_term();
            free(chip_walo);
            break;
        }
    }

    return 0;
}