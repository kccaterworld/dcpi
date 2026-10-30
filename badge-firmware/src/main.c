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
#include "buttons.h"

char alive; // 1 means keep living, 0 means shutdown

static void screen_preloop() {
    // Text test
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
}
static void leds_preloop() {
    leds_set_all(0, 0, 0); // all leds off
    leds_show();
}



static void input_checks(uint32_t now) {
    bool combo;
    combo = (button_held(BTN_A) || button_held(BTN_B));
    if (combo != lit_leds[5]) {
        if (combo) leds_set(5, 0, 128, 128);
        else     leds_set(5, 0, 0, 0);
        leds_show();
        lit_leds[5] = combo;
    }
    
    combo = (button_held(BTN_UP) || button_held(BTN_DOWN) || button_held(BTN_LEFT) || button_held(BTN_RIGHT));
    if (combo != lit_leds[4]) {
        if (combo) leds_set(4, 0, 128, 128);
        else     leds_set(4, 0, 0, 0);
        leds_show();
        lit_leds[4] = combo;
    }

    combo = (button_held(BTN_START) || button_held(BTN_SELECT));
    if (combo != lit_leds[6]) {
        if (combo) leds_set(6, 0, 128, 128);
        else     leds_set(6, 0, 0, 0);
        leds_show();
        lit_leds[6] = combo;
    }

    combo = button_held(BTN_START);
    if (combo != lit_leds[0]) {
        if (combo) leds_set(0, 0, 128, 128);
        else     leds_set(0, 0, 0, 0);
        leds_show();
        lit_leds[0] = combo;
    }

    combo = button_held(BTN_SELECT);
    if (combo != lit_leds[2]) {
        if (combo) leds_set(2, 0, 128, 128);
        else     leds_set(2, 0, 0, 0);
        leds_show();
        lit_leds[2] = combo;
    }

    combo = (button_held(BTN_FN) && button_held(BTN_SELECT) && button_held(BTN_START));
    if (combo) {
        alive = 0; // shutdown the badge
    }
}

int main() {
    // wakey wakey behavior
    hardware_init(); // first, so the badge stays on
    uart_init_serial();

    // Initializations and stuffs to dos before the game loop
    leds_init();
    buttons_init();
    display_init();
    text_init();

    leds_preloop();
    screen_preloop();

    // variables needed by the loop
    char line[128];
    alive = 1;

    // Game loop
    while (alive) {
        uint32_t now = to_ms_since_boot(get_absolute_time());

        buttons_update();
        input_checks(now);

        if (uart_read_line(line, sizeof line)) {
            printf("ok received: %s\n", line);
        }

        sleep_ms(1);
    }

    // shutdown behavior
    leds_set_all(0, 0, 0); // all leds off
    leds_show();
    display_clear( RGB(0, 0, 0) );
    display_flush();

    gpio_put(PIN_DISPLAY_BL, 0);
    gpio_put(PIN_POWER_HOLD, 0);
    // currently still requires a reset before power on
}
