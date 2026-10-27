#include "display.h"
#include "config.h"

// ST7789 command bytes
#define CMD_SWRESET 0x01
#define CMD_SLPOUT  0x11
#define CMD_NORON   0x13
#define CMD_INVOFF  0x20
#define CMD_INVON   0x21
#define CMD_DISPON  0x29
#define CMD_CASET   0x2A
#define CMD_RASET   0x2B
#define CMD_RAMWR   0x2C
#define CMD_MADCTL  0x36
#define CMD_COLMOD  0x3A

// --- Bit-banged SPI (mode 0, MSB first) ---

static void spi_write_byte(uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        gpio_put(PIN_DISPLAY_MOSI, (b >> i) & 1);
        busy_wait_at_least_cycles(DISPLAY_BITBANG_DELAY);
        gpio_put(PIN_DISPLAY_SCK, 1);
        busy_wait_at_least_cycles(DISPLAY_BITBANG_DELAY);
        gpio_put(PIN_DISPLAY_SCK, 0);
    }
}

static void write_command(uint8_t cmd) {
    gpio_put(PIN_DISPLAY_CS, 0);
    gpio_put(PIN_DISPLAY_DC, 0); // 0 = command
    spi_write_byte(cmd);
    gpio_put(PIN_DISPLAY_CS, 1);
}

static void write_data(const uint8_t *data, int len) {
    gpio_put(PIN_DISPLAY_CS, 0);
    gpio_put(PIN_DISPLAY_DC, 1); // 1 = data
    for (int i = 0; i < len; i++) {
        spi_write_byte(data[i]);
    }
    gpio_put(PIN_DISPLAY_CS, 1);
}

static void write_command_with_data(uint8_t cmd, const uint8_t *data, int len) {
    write_command(cmd);
    write_data(data, len);
}

// Select the rectangle that following pixel data fills, then start RAM write.
// Leaves CS low and DC high so the caller can stream pixels.
static void begin_pixels(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t cols[] = { x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF };
    uint8_t rows[] = { y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF };
    write_command_with_data(CMD_CASET, cols, 4);
    write_command_with_data(CMD_RASET, rows, 4);
    write_command(CMD_RAMWR);
    gpio_put(PIN_DISPLAY_CS, 0);
    gpio_put(PIN_DISPLAY_DC, 1);
}

static void push_pixel(uint16_t color) {
    spi_write_byte(color >> 8);
    spi_write_byte(color & 0xFF);
}

static void end_pixels(void) {
    gpio_put(PIN_DISPLAY_CS, 1);
}

// --- Public API ---

void display_init(void) {
    const uint pins[] = { PIN_DISPLAY_DC, PIN_DISPLAY_MOSI, PIN_DISPLAY_SCK,
                          PIN_DISPLAY_CS, PIN_DISPLAY_BL };
    for (int i = 0; i < 5; i++) {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], GPIO_OUT);
    }
    gpio_put(PIN_DISPLAY_CS, 1);
    gpio_put(PIN_DISPLAY_SCK, 0);
    gpio_put(PIN_DISPLAY_BL, 0);

    // The panel's reset line is tied high on the badge, so use a software reset.
    write_command(CMD_SWRESET);
    sleep_ms(150);
    write_command(CMD_SLPOUT);
    sleep_ms(120);

    uint8_t colmod = 0x55; // 16 bits per pixel (RGB565)
    write_command_with_data(CMD_COLMOD, &colmod, 1);

    uint8_t madctl = DISPLAY_MADCTL; // rotation / color order
    write_command_with_data(CMD_MADCTL, &madctl, 1);

    write_command(DISPLAY_INVERT ? CMD_INVON : CMD_INVOFF);
    write_command(CMD_NORON);
    sleep_ms(10);
    write_command(CMD_DISPON);
    sleep_ms(50);

    display_clear(COLOR_BLACK);
    gpio_put(PIN_DISPLAY_BL, 1);
}

void display_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT || w == 0 || h == 0) return;
    if (x + w > DISPLAY_WIDTH)  w = DISPLAY_WIDTH - x;
    if (y + h > DISPLAY_HEIGHT) h = DISPLAY_HEIGHT - y;

    begin_pixels(x, y, x + w - 1, y + h - 1);
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        push_pixel(color);
    }
    end_pixels();
}

void display_clear(uint16_t color) {
    display_fill_rect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, color);
}

void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    display_fill_rect(x, y, 1, 1, color);
}

void display_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *pixels) {
    if (x + w > DISPLAY_WIDTH || y + h > DISPLAY_HEIGHT || w == 0 || h == 0) return;

    begin_pixels(x, y, x + w - 1, y + h - 1);
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        push_pixel(pixels[i]);
    }
    end_pixels();
}
