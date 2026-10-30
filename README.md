# DefCon Badge Peripheral

An attempt to turn a Defcon 32 Badge into a peripheral for my headless Raspberry Pi 3B.
Probably a Rust daemon on the Pi and C firmware (Pico SDK)

## Specs

- **Microcontroller**: RP2350A
- **Display**: WS2812 (chat weve got touch screen nice)
- **Storage**: Micro SD card slot
- **Networking**: Nope (ir ig but like...no)
- **Audio**: Tiny crappy speaker
- **IR**: receive and transmit
- **GPIO**: 30 pins (0–29)
- **RAM**: 520KB on-chip SRAM
- **Flash**: 2MB
- **Processors**: Dual Arm Cortex-M33 or dual Hazard3 RISC-V @ 150MHz
- **Built in battery**: Yes
- **Ports**: USB-C and SAO header

## Features

System Monitor: CPU and Memory Stats (temp, speed, uptime)

Process Manager: See and terminate processes, resource consumption

Networking: Network scanning, connecting, disconnecting

SSH: Status, enable, disable

Systemd: Status, enable, disable, list

Files: Browse and read files

Power: Restart, shutdown

Dashboards: Custom configurable screens

LED Control: Controllable from other programs, fully programmable

Logs: System and service logs

## Plan

### Phase 1 — Hardware foundation

Establish communication, initialize the touchscreen, initialize the display, and control the LEDs.

### Phase 2 — Pi daemon

Implement the communication protocol, system monitoring, configuration loading, and local API.

### Phase 3 — Dynamic interface

Implement the widget renderer and Pi-defined dashboard layouts.

### Phase 4 — Management functionality

Add Wi-Fi, SSH, processes, services, filesystem browsing, and power controls.

### Phase 5 — LED automation

Implement effects, event triggers, priorities, and the local programming API.

### Phase 6 — Reliability

Add reconnect handling, configuration validation, permission controls, logging, and automatic daemon startup.

### Fancies

Custom scripting language for custom screens, animations, and widgets

## GPIO map (RP2350A, GPIO 0–29)

Based on board header i found, i think theres more...

| GPIO |         Board use          |  Config Mapping  | Notes                                             |
| ---- | -------------------------- | ---------------- | ------------------------------------------------- |
|   0  | External RAM chip select   | PIN_SRAM_CS      | Held high (off) by fs_init                        |
|   1  | Touch controller interrupt | UNDEFINED        | touch.c isnt even started yet                     |
|   2  | I2C SDA                    | UNDEFINED        | I2C1 Data. touch control? maybe SAO?              |
|   3  | I2C SCL                    | UNDEFINED        | I2C1 Clock                                        |
|   4  | RGB LEDs (WS2812)          | PIN_LEDS         | Driven by PIO0                                    |
|   5  | Display data/command       | PIN_DISPLAY_DC   | RS in docs for Register Select                    |
|   6  | Display data out           | PIN_DISPLAY_COPI | Bit-banged (not hardware SPI pins)                |
|   7  | IR transceiver shutdown    | UNDEFINED        | -                                                 |
|   8  | Display clock              | PIN_DISPLAY_SCK  | Bit-banged                                        |
|   9  | Display chip select        | PIN_DISPLAY_CS   | -                                                 |
|  10  | Display backlight          | PIN_DISPLAY_BL   | -                                                 |
|  11  | Power hold                 | PIN_POWER_HOLD   | High = stay on                                    |
|  12  | SPI bus CIPO/MISO          | PIN_SD_CIPO      | Hardware SPI1, shared bus                         |
|  13  | SD card chip select        | PIN_SD_CS        | Driven as a plain GPIO                            |
|  14  | SPI bus clock              | PIN_SD_SCK       | SPI1                                              |
|  15  | SPI bus COPI/MOSI          | PIN_SD_COPI      | SPI1                                              |
|  16  | Button Right               | PIN_BTN_RIGHT    | Pull-up, low = pressed                            |
|  17  | Button Down                | PIN_BTN_DOWN     | Pull-up, low = pressed                            |
|  18  | Button Up                  | PIN_BTN_UP       | Pull-up, low = pressed                            |
|  19  | Button Left                | PIN_BTN_LEFT     | Pull-up, low = pressed                            |
|  20  | Button B                   | PIN_BTN_B        | Pull-up, low = pressed                            |
|  21  | Button A                   | PIN_BTN_A        | Pull-up, low = pressed                            |
|  22  | Button Start               | PIN_BTN_START    | Pull-up, low = pressed                            |
|  23  | Button Select              | PIN_BTN_SELECT   | Pull-up, low = pressed                            |
|  24  | Button Fn                  | PIN_BTN_FN       | Pull-up, low = pressed                            |
|  25  | Speaker Out                | UNDEFINED        | Someday I'll implement an audio driver...         |
|  26  | ADC         | N/A              | ADC0          |
|  27  | IR RX                      | UNDEFINED        | Eventually going to be used (hehe fun things)     |
|  28  | IR TX                      | UNDEFINED        | Eventually going to be used (hehe fun things)     |
|  29  | ADC Battery Voltage Sensor | UNDEFINED        | Analog input for battery life on ADC3             |
