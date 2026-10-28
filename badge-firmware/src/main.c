#include <stdio.h>
#include "pico/stdlib.h"

#include "config.h"
#include "hardware.h"
#include "display.h"
#include "draw.h"
#include "text.h"
#include "leds.h"
#include "touch.h"
#include "uart.h"

// RGB565 8x8 image 
#define _ COLOR_BLACK
#define Y COLOR_YELLOW
static const uint16_t smiley[8 * 8] = {
    _, _, Y, Y, Y, Y, _, _,
    _, Y, Y, Y, Y, Y, Y, _,
    Y, Y, _, Y, Y, _, Y, Y,
    Y, Y, Y, Y, Y, Y, Y, Y,
    Y, _, Y, Y, Y, Y, _, Y,
    Y, Y, _, _, _, _, Y, Y,
    _, Y, Y, Y, Y, Y, Y, _,
    _, _, Y, Y, Y, Y, _, _,
};
#undef _
#undef Y

static void draw_screen() {
    display_clear(COLOR_BLACK);

    // Colored bars across the top: if these show in the wrong colors,
    // see the notes in config.h.
    draw_rect(0,   0, 80, 40, COLOR_RED);
    draw_filled_rect(80,  0, 80, 40, COLOR_GREEN);
    draw_filled_rect(160, 0, 80, 40, COLOR_BLUE);
    draw_filled_rect(240, 0, 80, 40, COLOR_WHITE);

    // Single pixels
    draw_pixel(10, 60, COLOR_WHITE);
    draw_pixel(12, 60, COLOR_WHITE);
    draw_pixel(14, 60, COLOR_WHITE);

    // The image, at (150, 110)
    draw_image(150, 110, 8, 8, smiley);
}

int main() {
    // wakey wakey behavior
    hardware_init(); // first, so the badge stays on
    uart_init_serial();

    leds_init();
    leds_set_all(0, 0, 0); // all leds off
    leds_show();

    display_init();
    //draw_screen();

    char line[128];
    uint32_t last_heartbeat = 0;
    char alive = 1; // 1 means keep living, 0 means shutdown
    int ledsflash = 0; // 1 means flash leds, 0 means don't flash

    // Every printable ASCII character (space through ~), then \x01, which has
    // no glyph and should show the diamond replacement character.
    char glyphs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
                    " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
                    "\x01"
                    "    "
                    "Hi Mr. DW, thanks   for teaching this!";
    for (int i = 0; glyphs[i] != '\0'; i++) {
        int x = 2 + (i % 20) * 16;
        int y = 4 + (i / 20) * 34;
        char note = glyphs[i];
        text_draw_character(note, x, y, 0.5, COLOR_WHITE);
    }
    display_flush();

    // Game loop
    while (alive) {
        if (uart_read_line(line, sizeof line)) {
            printf("ok received: %s\n", line);
        }

        uint32_t now = to_ms_since_boot(get_absolute_time());
        if ((now - last_heartbeat >= 5000) && ledsflash) {
            last_heartbeat = now;
            printf("badge alive (%lu ms)\n", (unsigned long)now);
            leds_set_all(0, 0, 8); // all leds blue
            leds_show();
            leds_set_all(0, 8, 0); // all leds green
            leds_show();
            leds_set_all(8, 0, 0); // all leds red
            leds_show();
            leds_set_all(0, 0, 0); // all leds off
            leds_show();
        }

        sleep_ms(1);
    }

    // shutdown behavior
}
