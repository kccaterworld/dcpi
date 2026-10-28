#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>

typedef enum {
    BTN_UP,
    BTN_DOWN,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_A,
    BTN_B,
    BTN_START,
    BTN_SELECT,
    BTN_FN,
    BTN_COUNT // number of buttons, keep last
} button_t;

void buttons_init();

// Read all buttons. Call once per main loop pass, before the checks below.
void buttons_update();

// True only on the update where the button went down.
bool button_pressed(button_t b);

// True only on the update where the button came back up.
bool button_released(button_t b);

// True for as long as the button is down.
bool button_held(button_t b);

#endif
