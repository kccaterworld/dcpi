#include <stdlib.h>
#include <math.h>

#include "display.h"
#include "config.h"

// Bit-banging SPI (mode 0, MSB first)

// Bit bang one (1) spi byte
static void spiwrite(uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        gpio_put(PIN_DISPLAY_MOSI, (b >> i) & 1); // Set MOSI pin to the current bit of the byte
        busy_wait_at_least_cycles(DISPLAY_BITBANG_DELAY);
        gpio_put(PIN_DISPLAY_SCK, 1); // 1 (high) = start clock
        busy_wait_at_least_cycles(DISPLAY_BITBANG_DELAY);
        gpio_put(PIN_DISPLAY_SCK, 0); // 0 (low) = stop clock
    }
}

// Write a command without a argument or data bytes
static void cmdwrite(uint8_t cmd) {
    gpio_put(PIN_DISPLAY_CS, 0); // 0 (low) = select/active
    gpio_put(PIN_DISPLAY_DC, 0); // 0 (low) = command
    spiwrite(cmd);
    gpio_put(PIN_DISPLAY_CS, 1); // 1 (high) = deselect/inactive
}

// Write n data bytes from *data, no command
static void datwriten(const uint8_t *data, int n) {
    gpio_put(PIN_DISPLAY_CS, 0); // 0 (low) = select/active
    gpio_put(PIN_DISPLAY_DC, 1); // 1 (high) = data
    for (int i = 0; i < n; i++) spiwrite(data[i]);
    gpio_put(PIN_DISPLAY_CS, 1); // 1 (high) = deselect/inactive
}

// Write a command followed by n data bytes
static void cmdwriten(uint8_t cmd, const uint8_t *data, int n) {
    cmdwrite(cmd);
    datwriten(data, n);
}

// Sets the row and col adresses for rect and sets pins/ram to prep for data
static void pixstart(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t cols[] = { x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF };
    uint8_t rows[] = { y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF };
    cmdwriten(CMD_CASET, cols, 4); // Set column range
    cmdwriten(CMD_RASET, rows, 4); // Set row range
    cmdwrite(CMD_RAMWR);
    gpio_put(PIN_DISPLAY_CS, 0); // 0 (low) = select/active
    gpio_put(PIN_DISPLAY_DC, 1); // 1 (high) = data
}
// One RGB565 pixel pushed to ram
static void pixpush(uint16_t color) {
    spiwrite(color >> 8);
    spiwrite(color & 0xFF);
}
// Deselect display and end the pixel stream
static void pixend(void) {
    gpio_put(PIN_DISPLAY_CS, 1); // 1 (high) = deselect/inactive
}

// DRAWING STUFFS YAY WAHOO
static uint16_t framebuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT]; // screen array
int min_x, min_y, max_x, max_y; // bounding box of changed pixels

void draw_pixel(int x, int y, uint16_t color) {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) return;
    framebuffer[y * DISPLAY_WIDTH + x] = color;
    if (x < min_x) min_x = x;
    if (x > max_x) max_x = x;
    if (y < min_y) min_y = y;
    if (y > max_y) max_y = y;
}

void draw_line(int x0, int y0, int x1, int y1, uint16_t color) {
    // Update bounding box
    int lo_x = x0 < x1 ? x0 : x1, hi_x = x0 < x1 ? x1 : x0, lo_y = y0 < y1 ? y0 : y1, hi_y = y0 < y1 ? y1 : y0;
    if (lo_x < 0) lo_x = 0;
    if (lo_y < 0) lo_y = 0;
    if (hi_x > DISPLAY_WIDTH - 1)  hi_x = DISPLAY_WIDTH - 1;
    if (hi_y > DISPLAY_HEIGHT - 1) hi_y = DISPLAY_HEIGHT - 1;
    if (lo_x <= hi_x && lo_y <= hi_y) { // skip if fully off-screen
        if (lo_x < min_x) min_x = lo_x;
        if (hi_x > max_x) max_x = hi_x;
        if (lo_y < min_y) min_y = lo_y;
        if (hi_y > max_y) max_y = hi_y;
    }
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
    if (x < 0) w += x, x = 0;
    if (y < 0) h += y, y = 0;
    if (x + w > DISPLAY_WIDTH)  w = DISPLAY_WIDTH - x;
    if (y + h > DISPLAY_HEIGHT) h = DISPLAY_HEIGHT - y;

    if (w <= 0 || h <= 0) return;
    if (x < min_x) min_x = x;
    if (x + w - 1 > max_x) max_x = x + w - 1;
    if (y < min_y) min_y = y;
    if (y + h - 1 > max_y) max_y = y + h - 1;

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

void draw_image(int x, int y, int w, int h, const uint16_t *pixels) {
    if (x + w > DISPLAY_WIDTH || y + h > DISPLAY_HEIGHT || w == 0 || h == 0) return;
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        framebuffer[(y + i / w) * DISPLAY_WIDTH + (x + i % w)] = pixels[i];
    }
    if (x < min_x) min_x = x;
    if (x + w - 1 > max_x) max_x = x + w - 1;
    if (y < min_y) min_y = y;
    if (y + h - 1 > max_y) max_y = y + h - 1;
}

// --- Public API ---

void display_init(void) {
    // Easily accessible display pins
    const uint pins[] = {
        PIN_DISPLAY_DC,
        PIN_DISPLAY_MOSI,
        PIN_DISPLAY_SCK,
        PIN_DISPLAY_CS,
        PIN_DISPLAY_BL
    };
    // Initialize display pins as outputs
    for (int i = 0; i < 5; i++) {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], GPIO_OUT);
    }
    gpio_put(PIN_DISPLAY_CS, 1);  // 1 (high) = inactive/deselected
    gpio_put(PIN_DISPLAY_SCK, 0); // 0 (low) = stop clock
    gpio_put(PIN_DISPLAY_BL, 0);  // 0 (low) = backlight off

    // The panel's reset line is tied high on the badge, so use a software reset.
    cmdwrite(CMD_SWRESET);
    sleep_ms(150);
    cmdwrite(CMD_SLPOUT);
    sleep_ms(120);

    uint8_t colmod = 0x55; // 16 bits per pixel (RGB565)
    cmdwriten(CMD_COLMOD, &colmod, 1);

    uint8_t madctl = DISPLAY_MADCTL; // rotation / color order
    cmdwriten(CMD_MADCTL, &madctl, 1);

    cmdwrite(DISPLAY_INVERT ? CMD_INVON : CMD_INVOFF); // Sets display inversion
    cmdwrite(CMD_NORON); // Normal display mode on
    sleep_ms(10);
    cmdwrite(CMD_DISPON); // Display itself on
    sleep_ms(50);

    display_clear(COLOR_BLACK); // Clear display to black
    gpio_put(PIN_DISPLAY_BL, 1); // 1 (high) = backlight on
}

void display_clear(uint16_t color) {
    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
        draw_filled_rect(0, y, DISPLAY_WIDTH, 1, color);
    }
}

void buffer_clear(void) {
    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
        for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
            framebuffer[y * DISPLAY_WIDTH + x] = COLOR_BLACK;
        }
    }
    min_x = DISPLAY_WIDTH;
    max_x = 0;
    min_y = DISPLAY_HEIGHT;
    max_y = 0;
}

void display_flush(void) {
    // check if box is empty
    if (min_x > max_x || min_y > max_y) return; // box is empty
    pixstart(min_x, min_y, max_x, max_y);
    for (uint32_t y = min_y; y <= max_y; y++) {
        for (uint32_t x = min_x; x <= max_x; x++) {
            pixpush(framebuffer[y * DISPLAY_WIDTH + x]);      // push pixel
            framebuffer[y * DISPLAY_WIDTH + x] = COLOR_BLACK; // clear pixel
        }
    }
    pixend();

}