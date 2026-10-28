#include <math.h>

#include "text.h"
#include "draw.h"

// Glyphs are drawn in their own units: x goes right, y goes UP from the
// baseline (0). Tallest letters reach GLYPH_TOP; descenders dip to about -14.
// The helpers below flip y for the screen (y down), scale by size, and offset
// so the (x, y) passed to text_draw_character is the top-left corner.
#define GLYPH_TOP     46 // top of tall letters, in glyph units
#define GLYPH_ADVANCE 28 // distance from one letter to the next, in glyph units

// Set by text_draw_character for the glyph being drawn
static int g_x, g_y;
static float g_size;
static uint16_t g_color;

static float glyph_sx(float gx) { return g_x + gx * g_size; }
static float glyph_sy(float gy) { return g_y + (GLYPH_TOP - gy) * g_size; }

static void glyph_line(float x0, float y0, float x1, float y1) {
    draw_line(lroundf(glyph_sx(x0)), lroundf(glyph_sy(y0)),
              lroundf(glyph_sx(x1)), lroundf(glyph_sy(y1)), g_color);
}

static void glyph_bezier(float x0, float y0, float x1, float y1,
                         float x2, float y2, float x3, float y3) {
    int steps = 4 + (int)(10 * g_size); // more segments for bigger text
    draw_bezier(glyph_sx(x0), glyph_sy(y0), glyph_sx(x1), glyph_sy(y1),
                glyph_sx(x2), glyph_sy(y2), glyph_sx(x3), glyph_sy(y3),
                steps, g_color);
}

// Filled dot, for the dots on i and j
static void glyph_dot(float cx, float cy, float r) {
    int sr = lroundf(r * g_size);
    if (sr < 1) sr = 1;
    draw_filled_ellipse(lroundf(glyph_sx(cx)), lroundf(glyph_sy(cy)), sr, sr, g_color);
}

void text_init() { // lowk figured this would be good to have but like...idk any more
    return;
}
void text_set_font() { // will not do anything or be used for now
    return;
}

// (x, y) is the top-left of the letter; size 1.0 = native (~46 px tall)
void text_draw_character(char c, int x, int y, float size, uint16_t color) {
    g_x = x, g_y = y, g_size = size, g_color = color;
    switch (c) { // we hardcode fonts like real men
            case 'a': {
                glyph_bezier(24,22, 14,29, 2,22, 2,13);
                glyph_bezier(2,13, 2,4, 14,-3, 24,4);
                glyph_line(24,26, 24,0);
                } break;
            case 'b': {
                glyph_line(2,46, 2,0);
                glyph_bezier(2,21, 12,29, 25,23, 25,13);
                glyph_bezier(25,13, 25,3, 12,-3, 2,5);
                } break;
            case 'c': {
                glyph_bezier(24,21, 16,28, 2,24, 2,13);
                glyph_bezier(2,13, 2,2, 16,-2, 24,5);
                } break;
            case 'd': {
                glyph_line(24,46, 24,0);
                glyph_bezier(24,21, 14,29, 1,23, 1,13);
                glyph_bezier(1,13, 1,3, 14,-3, 24,5);
                } break;
            case 'e': {
                glyph_line(1,14, 24,14);
                glyph_bezier(24,14, 24,27, 1,27, 1,13);
                glyph_bezier(1,13, 1,0, 14,-2, 23,5);
                } break;
            case 'f': {
                glyph_bezier(24,42, 20,48, 8,48, 8,36);
                glyph_line(8,36, 8,0);
                glyph_line(0,26, 18,26);
                } break;
            case 'g': {
                glyph_bezier(24,21, 14,29, 2,23, 2,13);
                glyph_bezier(2,13, 2,3, 14,-3, 24,4);
                glyph_line(24,26, 24,-4);
                glyph_bezier(24,-4, 24,-13, 10,-14, 3,-9);
                } break;
            case 'h': {
                glyph_line(2,46, 2,0);
                glyph_bezier(2,20, 10,28, 24,25, 24,14);
                glyph_line(24,14, 24,0);
                } break;
            case 'i': {
                glyph_line(13,26, 13,0);
                glyph_dot(13,37, 2);
                } break;
            case 'j': {
                glyph_line(16,26, 16,-4);
                glyph_bezier(16,-4, 16,-13, 6,-14, 1,-10);
                glyph_dot(16,37, 2);
                } break;
            case 'k': {
                glyph_line(2,46, 2,0);
                glyph_line(24,26, 2,10);
                glyph_line(10,16, 24,0);
                } break;
            case 'l': {
                glyph_line(10,46, 10,0);
                } break;
            case 'm': {
                glyph_line(2,26, 2,0);
                glyph_bezier(2,20, 8,28, 13,25, 13,14);
                glyph_line(13,14, 13,0);
                glyph_bezier(13,20, 19,28, 25,25, 25,14);
                glyph_line(25,14, 25,0);
                } break;
            case 'n': {
                glyph_line(2,26, 2,0);
                glyph_bezier(2,20, 10,28, 24,25, 24,14);
                glyph_line(24,14, 24,0);
                } break;
            case 'o': {
                glyph_bezier(2,13, 2,27, 24,27, 24,13);
                glyph_bezier(24,13, 24,-1, 2,-1, 2,13);
                } break;
            case 'p': {
                glyph_line(2,26, 2,-12);
                glyph_bezier(2,21, 12,29, 25,23, 25,13);
                glyph_bezier(25,13, 25,3, 12,-3, 2,5);
                } break;
            case 'q': {
                glyph_line(24,26, 24,-12);
                glyph_bezier(24,21, 14,29, 1,23, 1,13);
                glyph_bezier(1,13, 1,3, 14,-3, 24,5);
                } break;
            case 'r': {
                glyph_line(2,26, 2,0);
                glyph_bezier(2,19, 8,28, 18,27, 23,22);
                } break;
            case 's': {
                glyph_bezier(23,22, 18,28, 3,27, 4,19);
                glyph_bezier(4,19, 5,12, 21,15, 22,7);
                glyph_bezier(22,7, 23,-1, 8,-2, 3,4);
                } break;
            case 't': {
                glyph_line(10,40, 10,4);
                glyph_bezier(10,4, 10,-2, 18,0, 22,3);
                glyph_line(1,26, 20,26);
                } break;
            case 'u': {
                glyph_line(2,26, 2,6);
                glyph_bezier(2,6, 2,-3, 16,-3, 24,5);
                glyph_line(24,26, 24,0);
                } break;
            case 'v': {
                glyph_line(2,26, 13,0);
                glyph_line(13,0, 24,26);
                } break;
            case 'w': {
                glyph_line(1,26, 7,0);
                glyph_line(7,0, 13,18);
                glyph_line(13,18, 19,0);
                glyph_line(19,0, 25,26);
                } break;
            case 'x': {
                glyph_line(2,26, 24,0);
                glyph_line(2,0, 24,26);
                } break;
            case 'y': {
                glyph_line(2,26, 13,0);
                glyph_line(24,26, 7,-12);
                } break;
            case 'z': {
                glyph_line(2,26, 24,26);
                glyph_line(24,26, 2,0);
                glyph_line(2,0, 24,0);
                } break;
            case 'A': {
                glyph_line(1,0, 13,44);
                glyph_line(13,44, 25,0);
                glyph_line(5,16, 21,16);
                } break;
            case 'B': {
                glyph_line(3,44, 3,0);
                glyph_bezier(3,44, 18,44, 22,38, 22,33);
                glyph_bezier(22,33, 22,26, 14,23, 3,23);
                glyph_bezier(3,23, 18,23, 25,20, 25,12);
                glyph_bezier(25,12, 25,4, 16,0, 3,0);
                } break;
            case 'C': {
                glyph_bezier(24,38, 16,46, 2,40, 2,22);
                glyph_bezier(2,22, 2,4, 16,-2, 24,6);
                } break;
            case 'D': {
                glyph_line(3,44, 3,0);
                glyph_bezier(3,44, 20,44, 25,34, 25,22);
                glyph_bezier(25,22, 25,10, 20,0, 3,0);
                } break;
            case 'E': {
                glyph_line(3,44, 3,0);
                glyph_line(3,44, 24,44);
                glyph_line(3,22, 20,22);
                glyph_line(3,0, 24,0);
                } break;
            case 'F': {
                glyph_line(3,44, 3,0);
                glyph_line(3,44, 24,44);
                glyph_line(3,22, 20,22);
                } break;
            case 'G': {
                glyph_bezier(24,38, 16,46, 2,40, 2,22);
                glyph_bezier(2,22, 2,4, 18,-2, 24,8);
                glyph_line(24,8, 24,18);
                glyph_line(24,18, 15,18);
                } break;
            case 'H': {
                glyph_line(3,44, 3,0);
                glyph_line(24,44, 24,0);
                glyph_line(3,22, 24,22);
                } break;
            case 'I': {
                glyph_line(13,44, 13,0);
                } break;
            case 'J': {
                glyph_line(20,44, 20,8);
                glyph_bezier(20,8, 20,-3, 6,-4, 2,6);
                } break;
            case 'K': {
                glyph_line(3,44, 3,0);
                glyph_line(24,44, 3,20);
                glyph_line(10,26, 24,0);
                } break;
            case 'L': {
                glyph_line(3,44, 3,0);
                glyph_line(3,0, 24,0);
                } break;
            case 'M': {
                glyph_line(2,0, 2,44);
                glyph_line(2,44, 13,16);
                glyph_line(13,16, 24,44);
                glyph_line(24,44, 24,0);
                } break;
            case 'N': {
                glyph_line(3,0, 3,44);
                glyph_line(3,44, 24,0);
                glyph_line(24,0, 24,44);
                } break;
            case 'O': {
                glyph_bezier(2,22, 2,45, 25,45, 25,22);
                glyph_bezier(25,22, 25,-1, 2,-1, 2,22);
                } break;
            case 'P': {
                glyph_line(3,44, 3,0);
                glyph_bezier(3,44, 20,44, 25,38, 25,31);
                glyph_bezier(25,31, 25,24, 18,20, 3,20);
                } break;
            case 'Q': {
                glyph_bezier(2,22, 2,45, 25,45, 25,22);
                glyph_bezier(25,22, 25,-1, 2,-1, 2,22);
                glyph_line(17,10, 27,-4);
                } break;
            case 'R': {
                glyph_line(3,44, 3,0);
                glyph_bezier(3,44, 20,44, 25,38, 25,31);
                glyph_bezier(25,31, 25,24, 18,20, 3,20);
                glyph_line(12,20, 25,0);
                } break;
            case 'S': {
                glyph_bezier(24,38, 18,46, 3,44, 3,33);
                glyph_bezier(3,33, 3,22, 24,25, 24,12);
                glyph_bezier(24,12, 24,-1, 8,-2, 2,6);
                } break;
            case 'T': {
                glyph_line(13,44, 13,0);
                glyph_line(2,44, 25,44);
                } break;
            case 'U': {
                glyph_line(3,44, 3,12);
                glyph_bezier(3,12, 3,-3, 24,-3, 24,12);
                glyph_line(24,12, 24,44);
                } break;
            case 'V': {
                glyph_line(2,44, 13,0);
                glyph_line(13,0, 24,44);
                } break;
            case 'W': {
                glyph_line(1,44, 7,0);
                glyph_line(7,0, 13,30);
                glyph_line(13,30, 19,0);
                glyph_line(19,0, 25,44);
                } break;
            case 'X': {
                glyph_line(2,44, 24,0);
                glyph_line(2,0, 24,44);
                } break;
            case 'Y': {
                glyph_line(2,44, 13,22);
                glyph_line(24,44, 13,22);
                glyph_line(13,22, 13,0);
                } break;
            case 'Z': {
                glyph_line(2,44, 24,44);
                glyph_line(24,44, 2,0);
                glyph_line(2,0, 24,0);
                } break;
            case '0': {
                glyph_bezier(2,22, 2,45, 25,45, 25,22);
                glyph_bezier(25,22, 25,-1, 2,-1, 2,22);
                glyph_line(6,8, 21,36);
                } break;
            case '1': {
                glyph_line(5,34, 13,44);
                glyph_line(13,44, 13,0);
                glyph_line(4,0, 22,0);
                } break;
            case '2': {
                glyph_bezier(3,34, 4,45, 24,45, 24,32);
                glyph_bezier(24,32, 24,22, 6,12, 2,0);
                glyph_line(2,0, 24,0);
                } break;
            case '3': {
                glyph_bezier(3,38, 8,46, 24,44, 24,34);
                glyph_bezier(24,34, 24,26, 14,23, 8,23);
                glyph_bezier(8,23, 18,23, 25,20, 25,11);
                glyph_bezier(25,11, 25,0, 8,-3, 2,5);
                } break;
            case '4': {
                glyph_line(18,44, 1,12);
                glyph_line(1,12, 25,12);
                glyph_line(18,44, 18,0);
                } break;
            case '5': {
                glyph_line(22,44, 6,44);
                glyph_line(6,44, 5,26);
                glyph_bezier(5,26, 16,30, 25,24, 25,13);
                glyph_bezier(25,13, 25,1, 9,-3, 2,4);
                } break;
            case '6': {
                glyph_bezier(22,40, 10,47, 3,32, 3,14);
                glyph_bezier(3,14, 3,0, 24,0, 24,13);
                glyph_bezier(24,13, 24,24, 5,24, 3,14);
                } break;
            case '7': {
                glyph_line(2,44, 25,44);
                glyph_line(25,44, 9,0);
                } break;
            case '8': {
                glyph_bezier(13,22, 2,23, 2,44, 13,44);
                glyph_bezier(13,44, 25,44, 25,23, 13,22);
                glyph_bezier(13,22, 1,21, 1,0, 13,0);
                glyph_bezier(13,0, 26,0, 26,21, 13,22);
                } break;
            case '9': {
                glyph_bezier(5,4, 16,-3, 24,12, 24,30);
                glyph_bezier(24,30, 24,44, 3,44, 3,31);
                glyph_bezier(3,31, 3,20, 22,20, 24,30);
                } break;
            case '.': {
                glyph_dot(13,2, 2);
                } break;
            case ',': {
                glyph_dot(13,2, 2);
                glyph_line(14,1, 10,-7);
                } break;
            case '!': {
                glyph_line(13,44, 13,12);
                glyph_dot(13,2, 2);
                } break;
            case '?': {
                glyph_bezier(3,36, 4,46, 24,46, 23,34);
                glyph_bezier(23,34, 22,24, 13,24, 13,14);
                glyph_dot(13,2, 2);
                } break;
            case ':': {
                glyph_dot(13,24, 2);
                glyph_dot(13,2, 2);
                } break;
            case ';': {
                glyph_dot(13,24, 2);
                glyph_dot(13,2, 2);
                glyph_line(14,1, 10,-7);
                } break;
            case '\'': {
                glyph_line(13,44, 13,34);
                } break;
            case '"': {
                glyph_line(8,44, 8,34);
                glyph_line(18,44, 18,34);
                } break;
            case '-': {
                glyph_line(5,16, 21,16);
                } break;
            case '_': {
                glyph_line(1,-4, 25,-4);
                } break;
            case '(': {
                glyph_bezier(17,46, 5,34, 5,2, 17,-10);
                } break;
            case ')': {
                glyph_bezier(9,46, 21,34, 21,2, 9,-10);
                } break;
            case '[': {
                glyph_line(17,46, 9,46);
                glyph_line(9,46, 9,-10);
                glyph_line(9,-10, 17,-10);
                } break;
            case ']': {
                glyph_line(9,46, 17,46);
                glyph_line(17,46, 17,-10);
                glyph_line(17,-10, 9,-10);
                } break;
            case '{': {
                glyph_bezier(18,46, 10,46, 13,18, 6,18);
                glyph_bezier(6,18, 13,18, 10,-10, 18,-10);
                } break;
            case '}': {
                glyph_bezier(8,46, 16,46, 13,18, 20,18);
                glyph_bezier(20,18, 13,18, 16,-10, 8,-10);
                } break;
            case '/': {
                glyph_line(3,-2, 23,46);
                } break;
            case '\\': {
                glyph_line(3,46, 23,-2);
                } break;
            case '*': {
                glyph_line(13,40, 13,24);
                glyph_line(6,36, 20,28);
                glyph_line(6,28, 20,36);
                } break;
            case '#': {
                glyph_line(10,40, 8,4);
                glyph_line(19,40, 17,4);
                glyph_line(3,28, 24,28);
                glyph_line(2,16, 23,16);
                } break;
            case '%': {
                glyph_line(22,44, 4,0);
                glyph_bezier(2,36, 2,43, 12,43, 12,36);
                glyph_bezier(12,36, 12,29, 2,29, 2,36);
                glyph_bezier(14,8, 14,15, 24,15, 24,8);
                glyph_bezier(24,8, 24,1, 14,1, 14,8);
                } break;
            case '&': {
                glyph_bezier(25,0, 12,14, 6,26, 6,36);
                glyph_bezier(6,36, 6,46, 20,46, 20,36);
                glyph_bezier(20,36, 20,26, 2,22, 2,10);
                glyph_bezier(2,10, 2,-2, 18,-4, 24,14);
                } break;
            case '@': {
                glyph_bezier(8,22, 8,30, 18,30, 18,22);
                glyph_bezier(18,22, 18,14, 8,14, 8,22);
                glyph_line(18,30, 18,16);
                glyph_bezier(18,16, 18,10, 26,12, 26,22);
                glyph_bezier(26,22, 26,44, 1,44, 1,22);
                glyph_bezier(1,22, 1,2, 18,0, 24,6);
                } break;
            case ' ': { // whitespac
                } break;
            case '$': {
                glyph_bezier(23,36, 18,43, 4,42, 4,32);
                glyph_bezier(4,32, 4,22, 23,25, 23,13);
                glyph_bezier(23,13, 23,2, 8,1, 3,8);
                glyph_line(13,48, 13,-4);
                } break;
            case '<': {
                glyph_line(22,34, 4,20);
                glyph_line(4,20, 22,6);
                } break;
            case '>': {
                glyph_line(4,34, 22,20);
                glyph_line(22,20, 4,6);
                } break;
            case '=': {
                glyph_line(4,26, 22,26);
                glyph_line(4,14, 22,14);
                } break;
            case '^': {
                glyph_line(5,32, 13,44);
                glyph_line(13,44, 21,32);
                } break;
            case '+': {
                glyph_line(13,31, 13,9);
                glyph_line(4,20, 22,20);
                } break;
            case '`': {
                glyph_line(10,44, 16,36);
                } break;
            case '|': {
                glyph_line(13,46, 13,-10);
                } break;
            case '~': {
                glyph_bezier(3,18, 8,28, 18,12, 23,22);
                } break;
            default: { // unknown/error/null case
                glyph_line(13,46, 26,20);
                glyph_line(26,20, 13,-6);
                glyph_line(13,-6, 0,20);
                glyph_line(0,20, 13,46);
                glyph_bezier(8,30, 8,37, 18,37, 18,30);
                glyph_bezier(18,30, 18,24, 13,24, 13,18);
                glyph_dot(13,10, 1);
                } break;
        }
}
void text_draw_string(const char *str, int x, int y, float size, uint16_t color) {
    while (*str) {
        text_draw_character(*str++, x, y, size, color);
        x += lroundf(GLYPH_ADVANCE * size); // move right for the next letter
    }
}
void text_draw_string_n(const char *str, int x, int y, float size, uint16_t color, int n) {
    for (int i = 0; i < n; i++) {
        text_draw_character(str[i], x, y, size, color);
        x += lroundf(GLYPH_ADVANCE * size);
    }
}
