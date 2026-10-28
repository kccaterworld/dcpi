#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

// ST7789 (display) command bytes

// Reinit the screen (SoftWare RESET)
// Takes no args, Takes 150ms to reset, so wait
#define CMD_SWRESET 0x01

// Exit sleep (SLeep OUT)
// Takes no args, Takes 120ms to wake up, so wait
#define CMD_SLPOUT  0x11

// Normal display mode on (NORmal ON)
// Takes no args
#define CMD_NORON   0x13

// Turn off color inversion (INVert OFF)
// Takes no args
#define CMD_INVOFF  0x20

// Turn on color inversion (INVert ON)
// Takes no args
#define CMD_INVON   0x21

// Turn on display (DISPlay ON)
// Takes no args
#define CMD_DISPON  0x29

// Set collumn address (Column Address SET).
// Takes 4 bytes: 2 start bytes, 2 end bytes.
#define CMD_CASET   0x2A

// Set row address (Row Address SET)
// Takes 4 bytes: 2 start bytes, 2 end bytes.
#define CMD_RASET   0x2B

// Write to RAM (RAM Write)
// Data stream, keep going until CS high
#define CMD_RAMWR   0x2C

// Orientation and RGB/BGR order (Memory Access Data ConTroL)
// 1 byte, defined in config
#define CMD_MADCTL  0x36

// Color depth (COLor MODe)
// 1 byte, we're hardcoding 0x55, 16 bits per pixel (RGB565)
#define CMD_COLMOD  0x3A


// Colors are RGB565: 5 bits red, 6 green, 5 blue.
#define RGB(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0

void display_init(void);
void display_clear(uint16_t color);
void display_flush(void);

void buffer_clear(void);

void draw_pixel(int x, int y, uint16_t color);
void draw_line(int x0, int y0, int x1, int y1, uint16_t color);
void draw_rect(int x, int y, int w, int h, uint16_t color);
void draw_filled_rect(int x, int y, int w, int h, uint16_t color);
void draw_ellipse(int x, int y, int rx, int ry, uint16_t color);
void draw_filled_ellipse(int x, int y, int rx, int ry, uint16_t color);
void draw_n_gon(int x, int y, int r, int n, uint16_t color);
void draw_filled_n_gon(int x, int y, int r, int n, uint16_t color);
void draw_image(int x, int y, int w, int h, const uint16_t *pixels);

#endif
