// Puts font files' raw bytes into flash so text.c can parse them.
// Paths are relative to badge-firmware/assets (set in CMakeLists.txt).
// The bytes are NOT null-terminated: use the _start and _end labels.

    .section .rodata
    .global font_default_start
    .global font_default_end
font_default_start:
    .incbin "fonts/default.font"
font_default_end:
