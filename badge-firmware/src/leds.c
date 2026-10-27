#include "leds.h"
#include "config.h"
#include "hardware/clocks.h"
#include "hardware/sync.h"

// The LED signal is bit-banged (not PIO) so the timing is easy to tune.
// Sending all 9 LEDs takes well under 1 ms.

static uint32_t led_buffer[LED_COUNT]; // stored as GRB, the order WS2812 expects

static uint32_t t0h_cycles, t1h_cycles, tlow_cycles;

static uint32_t ns_to_cycles(uint32_t ns) {
    return (uint32_t)((uint64_t)clock_get_hz(clk_sys) * ns / 1000000000u);
}

void leds_set_timing(uint32_t t0h_ns, uint32_t t1h_ns, uint32_t tlow_ns) {
    t0h_cycles = ns_to_cycles(t0h_ns);
    t1h_cycles = ns_to_cycles(t1h_ns);
    tlow_cycles = ns_to_cycles(tlow_ns);
}

void leds_init(void) {
    gpio_init(PIN_LEDS);
    gpio_set_dir(PIN_LEDS, GPIO_OUT);
    gpio_put(PIN_LEDS, 0);
    leds_set_timing(LED_T0H_NS, LED_T1H_NS, LED_TLOW_NS);

    leds_set_all(0, 0, 0);
    leds_show();
}

void leds_set(int index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < 0 || index >= LED_COUNT) return;
    led_buffer[index] = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

void leds_set_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < LED_COUNT; i++) {
        leds_set(i, r, g, b);
    }
}

void leds_show(void) {
    // Interrupts off so USB can't stretch a pulse mid-bit.
    uint32_t irq_state = save_and_disable_interrupts();

    for (int i = 0; i < LED_COUNT; i++) {
        for (int bit = 23; bit >= 0; bit--) {
            bool one = (led_buffer[i] >> bit) & 1;
            gpio_put(PIN_LEDS, 1);
            busy_wait_at_least_cycles(one ? t1h_cycles : t0h_cycles);
            gpio_put(PIN_LEDS, 0);
            busy_wait_at_least_cycles(tlow_cycles);
        }
    }

    restore_interrupts(irq_state);

    // WS2812 latches the colors after the line stays low for a while.
    sleep_us(300);
}
