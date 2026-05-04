# Firmware

Arduino-ESP32 framework via PlatformIO. MP3/FLAC/AAC/WAV decode is software,
done by `esphome/ESP32-audioI2S` and clocked out over I2S to the PCM5102A.

## Build & flash

```sh
cd firmware
pio run                  # compile
pio run -t upload        # flash over USB-C (auto-DTR/RTS via USB-UART)
pio device monitor       # serial console at 115200
```

First boot creates `/state.json` in LittleFS to remember the last track and
volume across resets.

## SD card layout

```
/music/
    01 - some song.mp3
    02 - another.flac
    ...
```

FAT32, files up to 4 GiB, no subdirectories (kept simple — extend
`scanMusic()` if you want recursion).

## Controls

| Button     | Short press      | Long press (>600 ms)     |
| ---------- | ---------------- | ------------------------ |
| Play/Pause | toggle play      | (reserved: power-off)    |
| Next       | next track       | —                        |
| Prev       | previous track   | —                        |
| Vol +      | volume up        | —                        |
| Vol –      | volume down      | —                        |
| Menu       | cycle UI screen  | (reserved: settings)     |

## Notes on libraries

- `ESP32-audioI2S` v2.0.7 expects `setPinout(BCLK, LRC, DOUT)`; the API is
  stable on PlatformIO `espressif32` 6.x.
- `Adafruit_SSD1306` runs the OLED on the I2C bus we re-route to GPIO 21/4
  (default GPIO 22 conflicts with our I2S DOUT).
- The PCM5102A is set to format-detect mode by tying its FMT, XSMT and DEMP
  pins per the schematic — no driver-side configuration needed.
