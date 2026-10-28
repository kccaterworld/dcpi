#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

void text_init();
void text_set_font(); // will not do anything or be used for now

// (x, y) is the top-left of the text. size is a scale: 1.0 = native glyph
// size (~46 px tall letters, ~60 px including descenders), 0.5 = half.
void text_draw_character(char c, int x, int y, float size, uint16_t color);
void text_draw_string(const char *str, int x, int y, float size, uint16_t color);
void text_draw_string_n(const char *str, int x, int y, float size, uint16_t color, int n);

#endif