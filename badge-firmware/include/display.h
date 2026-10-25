#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
void display_init();
void display_clear(uint16_t color);

void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void display_draw_image(uint16_t x, uint16_t y, uint16_t* pixels, uint16_t color);

#endif