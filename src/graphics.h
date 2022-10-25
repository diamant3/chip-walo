#ifndef _GRAPHICS_H
#define _GRAPHICS_H

// graphic constants
#define SCREEN_SCALE 8 /* scaling factor for emulated pixel size */

// graphics functions
void gfx_init(void);
void gfx_update(Chip_walo *cw);
void gfx_term(void);
#endif