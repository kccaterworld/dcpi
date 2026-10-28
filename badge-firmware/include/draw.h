#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>

#include "display.h"

void draw_pixel(int x, int y, uint16_t color);
void draw_line(int x0, int y0, int x1, int y1, uint16_t color);
void draw_rect(int x, int y, int w, int h, uint16_t color);
void draw_filled_rect(int x, int y, int w, int h, uint16_t color);
void draw_ellipse(int x, int y, int rx, int ry, uint16_t color);
void draw_filled_ellipse(int x, int y, int rx, int ry, uint16_t color);
void draw_n_gon(int x, int y, int r, int n, uint16_t color);
void draw_filled_n_gon(int x, int y, int r, int n, uint16_t color);
void draw_bezier(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, int steps, uint16_t color);
void draw_hermite(float x0, float y0, float x1, float y1, float rx0, float ry0, float rx1, float ry1, int steps, uint16_t color);
void draw_image(int x, int y, int w, int h, const uint16_t *pixels);

#endif
