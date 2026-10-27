#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

void leds_init(void);

// Set one LED's color in the buffer (index 0 .. LED_COUNT-1). Not shown until leds_show().
void leds_set(int index, uint8_t r, uint8_t g, uint8_t b);
void leds_set_all(uint8_t r, uint8_t g, uint8_t b);

// Send the buffer to the LEDs.
void leds_show(void);

// Change the signal timing (normally set from config.h by leds_init).
void leds_set_timing(uint32_t t0h_ns, uint32_t t1h_ns, uint32_t tlow_ns);

#endif
