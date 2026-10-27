#include "hardware.h"
#include "config.h"

void hardware_init(void) {
    // The power button only enables the 3.3V supply while held. Driving this
    // pin high keeps it enabled after the button is released.
    gpio_init(PIN_POWER_HOLD);
    gpio_set_dir(PIN_POWER_HOLD, GPIO_OUT);
    gpio_put(PIN_POWER_HOLD, 1);
}
