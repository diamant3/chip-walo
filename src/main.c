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
        core_init();
        gfx_init();
        audio_init();
        core_load(argv[1]);
    } else {
        printf("Usage: xmake run chip-walo \"PATH/OF/THE/ROM\"\n");
        exit(0);
    }

    // cpu operation loop
    while (running) {
        key_detect();
        core_cycle();

        if (chip_walo->draw_flag == 1) {
            gfx_update();
            chip_walo->draw_flag = 0;
        }

        if (chip_walo->audio_flag == 1) {
            audio_beep();
            chip_walo->audio_flag = 0;
        }
    }

    gfx_deinit();
    audio_deinit();
    core_deinit();
    printf("[STATE] chip-walo stopped.\n");
    return 0;
}