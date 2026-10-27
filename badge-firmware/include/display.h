#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

// Colors are RGB565: 5 bits red, 6 green, 5 blue.
#define RGB565(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0

void display_init(void);
void display_clear(uint16_t color);

void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void display_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

// pixels is a w*h array of RGB565 colors, row by row.
void display_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *pixels);

#endif
