cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD defcon32_badge CACHE STRING "Target board")

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pi_badge C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

pico_sdk_init()

add_executable(pi_badge
    src/main.c
    src/hardware.c
    src/uart.c
    src/protocol.c
    src/display.c
    src/draw.c
    src/text.c
    src/touch.c
    src/battery.c
    src/leds.c
    src/buttons.c
    src/dashboard.c
    src/connection.c
)

target_include_directories(pi_badge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pi_badge
    pico_stdlib
    hardware_uart
    hardware_i2c
    hardware_spi
    hardware_gpio
    hardware_adc
    hardware_pwm
    hardware_pio
)

pico_enable_stdio_usb(pi_badge 1)
pico_enable_stdio_uart(pi_badge 0)

pico_add_extra_outputs(pi_badge)