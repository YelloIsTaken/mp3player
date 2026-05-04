# ESP32 MP3 Player ("Clip Jam"-class)

A pocket-sized MP3 player built around the ESP32-WROOM-32E. Form factor target:
**50 × 35 × 9 mm** (a touch thinner than the SanDisk Clip Jam at 41 × 36 × 15 mm).

Plays MP3/WAV/FLAC/AAC from a microSD card, drives a 0.96" OLED, charges via
USB-C, and runs ~10 hours on a 500 mAh LiPo.

## What's in this repo

| Path           | Contents                                                          |
| -------------- | ----------------------------------------------------------------- |
| `hardware/`    | KiCad 8 project: schematic, PCB, BOM, full netlist, pin map       |
| `case/`        | Parametric OpenSCAD case (top + bottom shell, button caps)        |
| `firmware/`    | PlatformIO project, Arduino-ESP32 framework, ESP32-audioI2S       |

## Block diagram

```
        ┌──────────┐  I2S   ┌──────────┐  L/R  ┌────────────┐  3.5 mm
        │          │───────▶│ PCM5102A │──────▶│ TPA6132A2  │────▶ jack
USB-C ─▶│  TP4056  │        │   DAC    │       │  HP amp    │
   │    │ charger  │        └──────────┘       └────────────┘
   │    └────┬─────┘             ▲ I2S
   │         │ VBAT          ┌───┴──────────────────────┐
   │      ┌──┴──┐ 3V3        │                          │
   │      │ LDO │───────────▶│      ESP32-WROOM-32E      │
   │      └─────┘            │                          │
   │                         └─┬────────┬────────┬──────┘
   │                       I2C │    SPI │   GPIO │
   │                           ▼        ▼        ▼
   │                       SSD1306   microSD   6× buttons,
   │                        OLED      card     HP-detect,
   │                                           battery ADC
   └──── slide switch ──── battery ── LDO enable
```

## Quick start

1. **Hardware:** open `hardware/mp3player.kicad_pro` in KiCad 8. Order PCB
   from a fab using gerbers exported from the PCB editor. BOM is in
   `hardware/BOM.md`, full net-list in `hardware/SCHEMATIC.md`.
2. **Case:** open `case/case.scad` in OpenSCAD, render and export STLs for
   `case_top`, `case_bottom`, and `button_caps`. PLA/PETG, 0.2 mm layer,
   3 perimeters, 20% infill.
3. **Firmware:** `cd firmware && pio run -t upload`. Drop MP3s into the root
   of a FAT32-formatted microSD card and insert.

See per-directory READMEs for deeper details.

## Pin map (ESP32-WROOM-32E)

| Pin   | Function          | Pin   | Function           |
| ----- | ----------------- | ----- | ------------------ |
| IO26  | I2S BCLK          | IO32  | BTN_PLAY/PAUSE     |
| IO25  | I2S LRCK          | IO33  | BTN_NEXT           |
| IO22  | I2S DOUT          | IO35  | BTN_PREV (in-only) |
| IO21  | I2C SDA (OLED)    | IO14  | BTN_VOL+           |
| IO4   | I2C SCL (OLED)    | IO39  | BTN_VOL- (in-only) |
| IO18  | SPI SCK (SD)      | IO36  | BTN_MENU (in-only) |
| IO23  | SPI MOSI (SD)     | IO13  | HP_DETECT          |
| IO19  | SPI MISO (SD)     | IO34  | BAT_ADC (in-only)  |
| IO5   | SD CS             | IO2   | STATUS LED         |

## License

Hardware: CERN-OHL-S v2. Firmware & case: MIT.
