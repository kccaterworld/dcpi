#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

// Loads the built-in font (assets/fonts/default.font). Call once at start-up,
// before drawing text; until then every character draws as the diamond.
void text_init();

// Parses a font file (data doesn't need to be null-terminated) and makes it
// the current font. Bad lines are skipped with a printf warning.
// Returns how many problems it found (0 = clean).
int text_set_font(const char *data, int len);

// (x, y) is the top-left of the text. size is a scale: 1.0 = native glyph
// size (~46 px tall letters, ~60 px including descenders), 0.5 = half.
void text_draw_character(char c, int x, int y, float size, uint16_t color);
void text_draw_string(const char *str, int x, int y, float size, uint16_t color);
void text_draw_string_n(const char *str, int x, int y, float size, uint16_t color, int n);

#endif