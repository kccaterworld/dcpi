#include "buttons.h"
#include "config.h"

// Same order as button_t in buttons.h.
static const uint button_pins[BTN_COUNT] = {
    PIN_BTN_UP, PIN_BTN_DOWN, PIN_BTN_LEFT, PIN_BTN_RIGHT,
    PIN_BTN_A, PIN_BTN_B, PIN_BTN_START, PIN_BTN_SELECT, PIN_BTN_FN,
};

static bool down_now[BTN_COUNT];  // state from the latest buttons_update()
static bool down_prev[BTN_COUNT]; // state from the one before

void buttons_init(void) {
    for (int i = 0; i < BTN_COUNT; i++) {
        gpio_init(button_pins[i]);
        gpio_set_dir(button_pins[i], GPIO_IN);
        gpio_pull_up(button_pins[i]);
        down_now[i] = false;
        down_prev[i] = false;
    }
}

void buttons_update(void) {
    for (int i = 0; i < BTN_COUNT; i++) {
        down_prev[i] = down_now[i];
        down_now[i] = !gpio_get(button_pins[i]); // low = pressed
    }
}

bool button_pressed(button_t b) {
    return down_now[b] && !down_prev[b];
}

bool button_released(button_t b) {
    return !down_now[b] && down_prev[b];
}

bool button_held(button_t b) {
    return down_now[b];
}
