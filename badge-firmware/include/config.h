#ifndef CONFIG_H
#define CONFIG_H

// Pin numbers come from the Pico SDK board file for this badge:
// pico-sdk/src/boards/include/boards/defcon32_badge.h
// (pulled in automatically because CMakeLists sets PICO_BOARD=defcon32_badge)
#include "pico/stdlib.h"

// Power
// must be high at reset to keep badge powered on
#define PIN_POWER_HOLD      DEFCON32_BADGE_SYS_POWER_CONTROL_PIN // GPIO 11

// RGB LEDs (WS2812, chained)
#define PIN_LEDS            DEFCON32_BADGE_WS2812_PIN // GPIO 4
#define LED_COUNT           9

// How long the data line stays high for a 0 bit and a 1 bit, and how long it
// stays low after each bit, in nanoseconds. The badge's level shifter rises
// slowly, so these are longer than the WS2812 datasheet values.
// Not yet confirmed: set LED_TIMING_TEST to 1 to cycle through candidates.
#define LED_T0H_NS          375
#define LED_T1H_NS          750
#define LED_TLOW_NS         500
#define LED_TIMING_TEST     1

// Buttons
// Each button shorts its pin to ground; no pull-ups on the board, so
// buttons.c turns on the internal ones. Pressed = low.
// (Boot, Reset and Power aren't on GPIOs, so code can't read them.)
#define PIN_BTN_UP      DEFCON32_BADGE_SW_UP_PIN     // GPIO 18
#define PIN_BTN_DOWN    DEFCON32_BADGE_SW_DOWN_PIN   // GPIO 17
#define PIN_BTN_LEFT    DEFCON32_BADGE_SW_LEFT_PIN   // GPIO 19
#define PIN_BTN_RIGHT   DEFCON32_BADGE_SW_RIGHT_PIN  // GPIO 16

#define PIN_BTN_A       DEFCON32_BADGE_SW_A_PIN      // GPIO 21
#define PIN_BTN_B       DEFCON32_BADGE_SW_B_PIN      // GPIO 20

#define PIN_BTN_START   DEFCON32_BADGE_SW_START_PIN  // GPIO 22
#define PIN_BTN_SELECT  DEFCON32_BADGE_SW_SELECT_PIN // GPIO 23
#define PIN_BTN_FN      DEFCON32_BADGE_SW_FN_PIN     // GPIO 24

// Display (2.4" 240x320, ST7789 per the schematic note)
// These pins don't match the chip's hardware SPI, so display.c bit-bangs them.
#define PIN_DISPLAY_DC      DEFCON32_BADGE_DISPLAY_RS_PIN  // GPIO 5 (data/command)
#define PIN_DISPLAY_MOSI    DEFCON32_BADGE_DISPLAY_DO_PIN  // GPIO 6
#define PIN_DISPLAY_SCK     DEFCON32_BADGE_DISPLAY_SCK_PIN // GPIO 8
#define PIN_DISPLAY_CS      DEFCON32_BADGE_DISPLAY_CS_PIN  // GPIO 9
#define PIN_DISPLAY_BL      DEFCON32_BADGE_DISPLAY_BL_PIN  // GPIO 10 (backlight, high = on)

// Not confirmed for this panel. If the picture looks wrong, change these:
//   colors look inverted (black shows white)  -> flip DISPLAY_INVERT
//   red and blue swapped                      -> toggle 0x08 in DISPLAY_MADCTL
//   image upside down / mirrored              -> try 0x60, 0xA0, 0xE0, 0x20
//   (for portrait, use 0x00 and swap WIDTH/HEIGHT)
#define DISPLAY_WIDTH       320
#define DISPLAY_HEIGHT      240
#define DISPLAY_MADCTL      0x60
#define DISPLAY_INVERT      0

// Busy-wait cycles per half clock in the bit-banged SPI. Raise it if the
// screen shows garbage; lower it for speed.
#define DISPLAY_BITBANG_DELAY 8

#endif
