#ifndef _GRAPHICS_H
#define _GRAPHICS_H

// graphic constants
#define SCREEN_SCALE  8    /* scaling factor for emulated pixel size */
#define SCREEN_WIDTH  64   /* width of the display */
#define SCREEN_HEIGHT 32   /* height of the display */
#define SCREEN_SIZE   2048 /* size of displayed graphics in pixel (64 x 32) */

// graphics functions
void gfx_init(void);
void gfx_update(Chip_walo *cw);
void gfx_term(void);
#endif