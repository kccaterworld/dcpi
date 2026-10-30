#include "display.h"
#include "config.h"

// Bit-banging SPI (mode 0, MSB first)

// Bit bang one (1) spi byte
static void spiwrite(uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        gpio_put(PIN_DISPLAY_COPI, (b >> i) & 1); // Set MOSI pin to the current bit of the byte
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
static void pixend() {
    gpio_put(PIN_DISPLAY_CS, 1); // 1 (high) = deselect/inactive
}

static uint16_t framebuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT]; // screen array
static int min_x = DISPLAY_WIDTH, min_y = DISPLAY_HEIGHT, max_x = -1, max_y = -1; // bounding box

// --- Public API ---

void display_init() {
    // Easily accessible display pins
    const uint pins[] = {
        PIN_DISPLAY_DC,
        PIN_DISPLAY_COPI,
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
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) framebuffer[i] = color;
    display_mark_dirty(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
}

uint16_t *display_framebuffer(void) {
    return framebuffer;
}

void display_mark_dirty(int x0, int y0, int x1, int y1) {
    // Clamp to the screen
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > DISPLAY_WIDTH - 1)  x1 = DISPLAY_WIDTH - 1;
    if (y1 > DISPLAY_HEIGHT - 1) y1 = DISPLAY_HEIGHT - 1;
    if (x0 > x1 || y0 > y1) return; // fully off-screen

    if (x0 < min_x) min_x = x0;
    if (x1 > max_x) max_x = x1;
    if (y0 < min_y) min_y = y0;
    if (y1 > max_y) max_y = y1;
}

void buffer_clear() {
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

void display_flush() {
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