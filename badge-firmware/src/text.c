#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static int glyph_steps(void) { return 4 + (int)(10 * g_size); } // more segments for bigger text

static void glyph_bezier(float x0, float y0, float x1, float y1,
                         float x2, float y2, float x3, float y3) {
    draw_bezier(glyph_sx(x0), glyph_sy(y0), glyph_sx(x1), glyph_sy(y1),
                glyph_sx(x2), glyph_sy(y2), glyph_sx(x3), glyph_sy(y3),
                glyph_steps(), g_color);
}

// Directions (r) are only scaled, not moved, and y is flipped like points are
static void glyph_hermite(float x0, float y0, float x1, float y1,
                          float rx0, float ry0, float rx1, float ry1) {
    draw_hermite(glyph_sx(x0), glyph_sy(y0), glyph_sx(x1), glyph_sy(y1),
                 rx0 * g_size, -ry0 * g_size, rx1 * g_size, -ry1 * g_size,
                 glyph_steps(), g_color);
}

// Filled dot, for the dots on i and j
static void glyph_dot(float cx, float cy, float r) {
    int sr = lroundf(r * g_size);
    if (sr < 1) sr = 1;
    draw_filled_ellipse(lroundf(glyph_sx(cx)), lroundf(glyph_sy(cy)), sr, sr, g_color);
}

// Diamond with a question mark, for characters the font doesn't have.
// Kept in code so it still works if the font file is missing or broken.
static void glyph_replacement(void) {
    glyph_line(13,46, 26,20);
    glyph_line(26,20, 13,-6);
    glyph_line(13,-6, 0,20);
    glyph_line(0,20, 13,46);
    glyph_bezier(8,30, 8,37, 18,37, 18,30);
    glyph_bezier(18,30, 18,24, 13,24, 13,18);
    glyph_dot(13,10, 1);
}

// --- Font storage ---
// text_set_font parses a font file once into this table; drawing only reads it.

enum { STROKE_LINE, STROKE_BEZIER, STROKE_HERMITE, STROKE_DOT };

typedef struct {
    uint8_t type;   // STROKE_*
    int8_t args[8]; // glyph units; only the first nargs are used
} stroke_t;

// Stroke keywords in the font file and how many numbers each takes
static const struct {
    const char *name;
    uint8_t type;
    int nargs;
} stroke_kinds[] = {
    { "line",    STROKE_LINE,    4 },
    { "bezier",  STROKE_BEZIER,  8 },
    { "hermite", STROKE_HERMITE, 8 },
    { "dot",     STROKE_DOT,     3 },
};
#define STROKE_KIND_COUNT (sizeof stroke_kinds / sizeof stroke_kinds[0])

#define MAX_STROKES   512 // whole font; the default uses about 250
#define MAX_LINE_LEN  128

static stroke_t strokes[MAX_STROKES];
static int stroke_total;
static bool     glyph_defined[128]; // false = draw the replacement
static uint16_t glyph_first[128];   // index of the glyph's first stroke
static uint8_t  glyph_count[128];   // how many strokes it has

// The default font file's bytes, put in flash by font_data.S
extern const char font_default_start[], font_default_end[];

// True if the word (not null-terminated, len chars) equals name
static bool word_is(const char *word, int len, const char *name) {
    return (int)strlen(name) == len && memcmp(word, name, len) == 0;
}

int text_set_font(const char *data, int len) {
    memset(glyph_defined, 0, sizeof glyph_defined);
    stroke_total = 0;

    int errors = 0;
    int line_no = 0;
    int cur = -1;        // glyph being defined; -1 = none, -2 = skipping a bad one
    const char *p = data, *end = data + len;
    char buf[MAX_LINE_LEN];

    while (p < end) {
        // Copy one line into buf (the data isn't null-terminated)
        const char *nl = memchr(p, '\n', end - p);
        const char *line_end = nl ? nl : end;
        int n = line_end - p;
        p = nl ? nl + 1 : end;
        line_no++;
        if (n >= MAX_LINE_LEN) {
            printf("font: line %d: too long, skipped\n", line_no);
            errors++;
            continue;
        }
        memcpy(buf, line_end - n, n);
        buf[n] = '\0';

        // Trim trailing spaces/tabs/\r (Windows line endings) and leading spaces
        while (n > 0 && (buf[n - 1] == ' ' || buf[n - 1] == '\t' || buf[n - 1] == '\r')) buf[--n] = '\0';
        char *s = buf;
        while (*s == ' ' || *s == '\t') s++;
        if (*s == '\0' || *s == '#') continue; // blank line or comment

        // Split off the first word; s then points at the rest
        char *word = s;
        while (*s && *s != ' ' && *s != '\t') s++;
        int word_len = s - word;
        while (*s == ' ' || *s == '\t') s++;

        if (word_is(word, word_len, "gstart")) {
            if (cur >= 0) {
                printf("font: line %d: gstart before gend of the previous glyph\n", line_no);
                errors++;
            }
            // One character = that character; more = its decimal code
            long code;
            if (s[0] != '\0' && s[1] == '\0') {
                code = (unsigned char)s[0];
            } else {
                char *num_end;
                code = strtol(s, &num_end, 10);
                if (*s == '\0' || *num_end != '\0') code = -1;
            }
            if (code < 0 || code > 127) {
                printf("font: line %d: bad glyph name \"%s\", skipping glyph\n", line_no, s);
                errors++;
                cur = -2;
                continue;
            }
            cur = (int)code;
            glyph_defined[cur] = true; // a later duplicate replaces this one
            glyph_first[cur] = stroke_total;
            glyph_count[cur] = 0;
        } else if (word_is(word, word_len, "gend")) {
            if (cur == -1) {
                printf("font: line %d: gend without gstart\n", line_no);
                errors++;
            }
            cur = -1;
        } else {
            if (cur == -2) continue; // inside a glyph we're skipping
            if (cur == -1) {
                printf("font: line %d: stroke outside a glyph, skipped\n", line_no);
                errors++;
                continue;
            }

            // Which stroke is it?
            int kind = -1;
            for (int k = 0; k < (int)STROKE_KIND_COUNT; k++) {
                if (word_is(word, word_len, stroke_kinds[k].name)) kind = k;
            }
            if (kind < 0) {
                printf("font: line %d: unknown stroke \"%.*s\", skipped\n", line_no, word_len, word);
                errors++;
                continue;
            }

            // Read exactly nargs numbers, each fitting in an int8_t
            stroke_t st = { .type = stroke_kinds[kind].type };
            int nargs = stroke_kinds[kind].nargs;
            bool ok = true;
            for (int a = 0; a < nargs && ok; a++) {
                char *num_end;
                long v = strtol(s, &num_end, 10);
                if (num_end == s || v < -128 || v > 127) ok = false;
                st.args[a] = (int8_t)v;
                s = num_end;
            }
            while (*s == ' ' || *s == '\t') s++;
            if (!ok || *s != '\0') {
                printf("font: line %d: %s needs %d numbers from -128 to 127, skipped\n",
                       line_no, stroke_kinds[kind].name, nargs);
                errors++;
                continue;
            }

            if (stroke_total >= MAX_STROKES || glyph_count[cur] == UINT8_MAX) {
                printf("font: line %d: too many strokes, skipped\n", line_no);
                errors++;
                continue;
            }
            strokes[stroke_total++] = st;
            glyph_count[cur]++;
        }
    }

    if (cur >= 0) {
        printf("font: missing gend at end of file\n"); // the glyph is still kept
        errors++;
    }
    return errors;
}

void text_init() {
    text_set_font(font_default_start, font_default_end - font_default_start);
}

// (x, y) is the top-left of the letter; size 1.0 = native (~46 px tall)
void text_draw_character(char c, int x, int y, float size, uint16_t color) {
    g_x = x, g_y = y, g_size = size, g_color = color;

    unsigned char uc = (unsigned char)c;
    if (uc >= 128 || !glyph_defined[uc]) {
        glyph_replacement();
        return;
    }

    for (int i = 0; i < glyph_count[uc]; i++) {
        const stroke_t *st = &strokes[glyph_first[uc] + i];
        const int8_t *a = st->args;
        switch (st->type) {
            case STROKE_LINE:    glyph_line(a[0], a[1], a[2], a[3]); break;
            case STROKE_BEZIER:  glyph_bezier(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]); break;
            case STROKE_HERMITE: glyph_hermite(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]); break;
            case STROKE_DOT:     glyph_dot(a[0], a[1], a[2]); break;
        }
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
