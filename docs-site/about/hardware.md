# Hardware

The firmware runs on the [ClockworkPi PicoCalc](https://www.clockworkpi.com/picocalc):
a swappable Raspberry Pi Pico module mounted on the ClockworkPi v2.0
mainboard, which provides the display, keyboard, audio, storage, and
power peripherals shared across all Pico module variants.

## Pico module comparison

| | Pico 1 H (RP2040) | Pico 2 H (RP2350) |
|---|---|---|
| CPU | 2x Cortex-M0+ @ 133 MHz (overclockable to 200-250 MHz) | 2x Cortex-M33 @ 150 MHz |
| SRAM | 264 KB | 520 KB |
| Flash | 2 MB | 4 MB |
| FPU | None (software float via ROM routines) | Single-precision hardware |

Both boards are supported and hardware-verified; either works.

## Mainboard peripherals (shared across both boards)

- **Display**: 4-inch IPS LCD, 320x320 pixels, RGB565, over 4-wire SPI.
- **Keyboard**: 67-key QWERTY, scanned by an onboard STM32
  co-processor and read over I2C.
- **Memory**: 8 MB PSRAM (SPI-attached) plus a full-size SD card slot
  (32 GB card included) for programs, variables, history, and
  configuration.
- **Audio**: a PWM-driven piezo buzzer.
- **Power**: an 18650 Li-ion cell with USB-C charging; the status bar
  shows a cached battery reading, refreshed periodically.

See the main repository's `docs/hardware.md` for the full developer-
facing hardware reference (pinouts, benchmarks, schematics).
