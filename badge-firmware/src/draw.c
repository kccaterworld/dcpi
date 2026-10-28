#include <stdlib.h>
#include <math.h>

#include "draw.h"
#include "display.h"
#include "config.h"

void draw_pixel(int x, int y, uint16_t color) {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) return;
    display_framebuffer()[y * DISPLAY_WIDTH + x] = color;
    display_mark_dirty(x, y, x, y);
}

void draw_line(int x0, int y0, int x1, int y1, uint16_t color) {
    uint16_t *framebuffer = display_framebuffer();
    int lo_x = x0 < x1 ? x0 : x1, hi_x = x0 < x1 ? x1 : x0, lo_y = y0 < y1 ? y0 : y1, hi_y = y0 < y1 ? y1 : y0;
    display_mark_dirty(lo_x, lo_y, hi_x, hi_y);

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1, dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1, err = dx + dy;
    for (;;) {
        if (x0 >= 0 && x0 < DISPLAY_WIDTH && y0 >= 0 && y0 < DISPLAY_HEIGHT) framebuffer[y0 * DISPLAY_WIDTH + x0] = color;
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) err += dy, x0 += sx;
        if (e2 <= dx) err += dx, y0 += sy;
    }
}

void draw_rect(int x, int y, int w, int h, uint16_t color) {
    draw_line(x, y, x + w - 1, y, color);
    draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
    draw_line(x + w - 1, y + h - 1, x, y + h - 1, color);
    draw_line(x, y + h - 1, x, y, color);
}
void draw_filled_rect(int x, int y, int w, int h, uint16_t color) {
    uint16_t *framebuffer = display_framebuffer();
    if (x < 0) w += x, x = 0;
    if (y < 0) h += y, y = 0;
    if (x + w > DISPLAY_WIDTH)  w = DISPLAY_WIDTH - x;
    if (y + h > DISPLAY_HEIGHT) h = DISPLAY_HEIGHT - y;

    if (w <= 0 || h <= 0) return;
    display_mark_dirty(x, y, x + w - 1, y + h - 1);

    for (int row = y; row < y + h; row++) {
        for (int i = 0; i < w; i++) framebuffer[row * DISPLAY_WIDTH + x + i] = color;
    }
}

void draw_ellipse(int x, int y, int rx, int ry, uint16_t color) {
    int x0 = rx, y0 = 0, err = rx * rx * (1 - ry) + ry * ry * (1 - rx);
    while (x0 >= y0) {
        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x0 > rx) x0 = rx;
        if (y0 > ry) y0 = ry;
        draw_line(x - x0, y - y0, x + x0, y - y0, color);
        draw_line(x - x0, y + y0, x + x0, y + y0, color);
        draw_line(x - y0, y - x0, x - y0, y + x0, color);
        draw_line(x + y0, y - x0, x + y0, y + x0, color);
        if (err < 0) y0++, err += ry * ry * (2 * y0 + 1);
        else x0--, err += rx * rx * (1 - 2 * x0);
    }
}

void draw_filled_ellipse(int x, int y, int rx, int ry, uint16_t color) {
    for (int y0 = -ry; y0 <= ry; y0++) {
        int x0 = rx * sqrt(1 - (y0 * y0) / (ry * ry));
        draw_line(x - x0, y + y0, x + x0, y + y0, color);
    }
}

void draw_n_gon(int x, int y, int r, int n, uint16_t color) {
    for (int i = 0; i < n; i++) {
        int x1 = x + r * cos(2 * M_PI * i / n);
        int y1 = y + r * sin(2 * M_PI * i / n);
        int x2 = x + r * cos(2 * M_PI * (i + 1) / n);
        int y2 = y + r * sin(2 * M_PI * (i + 1) / n);
        draw_line(x1, y1, x2, y2, color);
    }
}

void draw_filled_n_gon(int x, int y, int r, int n, uint16_t color) {
    for (int i = 0; i < n; i++) {
        int x1 = x + r * cos(2 * M_PI * i / n);
        int y1 = y + r * sin(2 * M_PI * i / n);
        int x2 = x + r * cos(2 * M_PI * (i + 1) / n);
        int y2 = y + r * sin(2 * M_PI * (i + 1) / n);
        draw_line(x1, y1, x2, y2, color);
    }
}

static void draw_cubic(float ax, float bx, float cx, float dx,
                       float ay, float by, float cy, float dy,
                       int steps, uint16_t color) {
    if (steps < 1) steps = 1;
    int px = lroundf(dx), py = lroundf(dy); // t = 0
    for (int i = 1; i <= steps; i++) {
        float t = (float)i / steps;
        // Horner's form: same as a*t^3 + b*t^2 + c*t + d, fewer multiplies
        int nx = lroundf(((ax * t + bx) * t + cx) * t + dx);
        int ny = lroundf(((ay * t + by) * t + cy) * t + dy);
        draw_line(px, py, nx, ny, color);
        px = nx, py = ny;
    }
}

void draw_bezier(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, int steps, uint16_t color) {
    draw_cubic(-x0 + 3 * x1 - 3 * x2 + x3,   // Third degree
               3 * x0 - 6 * x1 + 3 * x2,     // Second Degree
               -3 * x0 + 3 * x1,             // First Degree
               x0,                           // Constant
               -y0 + 3 * y1 - 3 * y2 + y3,
               3 * y0 - 6 * y1 + 3 * y2,
               -3 * y0 + 3 * y1,
               y0,
               steps, color);
}

void draw_hermite(float x0, float y0, float x1, float y1, float rx0, float ry0, float rx1, float ry1, int steps, uint16_t color) {
    draw_cubic(2 * x0 - 2 * x1 + rx0 + rx1,      // Third Degree
               -3 * x0 + 3 * x1 - 2 * rx0 - rx1, // Second Degree
               rx0,                              // First Degree
               x0,                               // Constant
               2 * y0 - 2 * y1 + ry0 + ry1,
               -3 * y0 + 3 * y1 - 2 * ry0 - ry1,
               ry0,
               y0,
               steps, color);
}

void draw_image(int x, int y, int w, int h, const uint16_t *pixels) {
    if (x < 0 || y < 0 || w <= 0 || h <= 0) return;

    uint16_t *framebuffer = display_framebuffer();
    if (x + w > DISPLAY_WIDTH || y + h > DISPLAY_HEIGHT || w == 0 || h == 0) return;
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        framebuffer[(y + i / w) * DISPLAY_WIDTH + (x + i % w)] = pixels[i];
    }
    display_mark_dirty(x, y, x + w - 1, y + h - 1);
}
