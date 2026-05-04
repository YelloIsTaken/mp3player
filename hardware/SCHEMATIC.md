# Full schematic / netlist

This is the **authoritative wiring** for the board. The KiCad `.kicad_sch`
file ships as a block-level overview; recreate or extend it from this
netlist.

## Power tree

```
USB-C VBUS ──FB1──┬── U4.VBUS (TP4056 input)
                  └── (no other load: USB is charge-only)

U4.BAT ── J4.+ (battery +) ─── VBAT net
                                │
                                ├── SW7 (slide switch) ── VBAT_SW ── U5.VIN (LDO)
                                │
                                └── R8 ── BAT_SENSE ── R9 ── GND   (50 % divider)
                                                │
                                                └── ESP32 IO34 (BAT_ADC)

U5.VOUT (3.3 V) ── 3V3 net ── ESP32 3V3, OLED VCC, U2 DVDD/AVDD/CPVDD,
                              U3 VCC, microSD VCC, pull-ups
```

- **TP4056 ISET = 500 mA** with R1 = 2.4 kΩ on PROG.
- **CHRG**: D1 (red) via R12 to 3V3.  **STDBY**: D2 (green) via R13.
- **U6 P-MOSFET** between battery + and the rest of the load (source =
  battery, drain = VBAT_LOAD), gate pulled to VBAT through 100 kΩ and
  switched by USB VBUS via a level-shift NPN — provides automatic
  power-path so the device runs from USB while charging without
  back-feeding the battery. (Optional; many designs skip it and just rely
  on the TP4056's reverse-blocking.)

## ESP32 module (U1)

| ESP32 pin | Net          | Goes to                                          |
| --------- | ------------ | ------------------------------------------------ |
| 3V3       | 3V3          | Decoupling C3 (100 nF) + C1 (10 µF)              |
| GND (×4)  | GND          |                                                  |
| EN        | EN           | R15 10 kΩ pull-up to 3V3, RESET button (opt)     |
| GPIO 0    | BOOT         | Test point (programming pull-down)               |
| GPIO 2    | LED_STATUS   | R14 1 kΩ → D3 (status LED) → GND                 |
| GPIO 4    | I2C_SCL      | OLED SCL, R19 4.7 kΩ pull-up                     |
| GPIO 5    | SD_CS        | microSD CS                                       |
| GPIO 13   | HP_DETECT    | J3 switch contact                                |
| GPIO 14   | BTN_VOLUP    | SW4 → GND, internal pull-up                      |
| GPIO 18   | SD_SCK       | microSD SCK                                      |
| GPIO 19   | SD_MISO      | microSD MISO (DAT0)                              |
| GPIO 21   | I2C_SDA      | OLED SDA, R18 4.7 kΩ pull-up                     |
| GPIO 22   | I2S_DOUT     | U2 DIN (PCM5102A pin 14)                         |
| GPIO 23   | SD_MOSI      | microSD MOSI (CMD)                               |
| GPIO 25   | I2S_LRCK     | U2 LRCK (pin 15)                                 |
| GPIO 26   | I2S_BCLK     | U2 BCLK (pin 13)                                 |
| GPIO 32   | BTN_PLAY     | SW1 → GND, internal pull-up                      |
| GPIO 33   | BTN_NEXT     | SW2 → GND, internal pull-up                      |
| GPIO 34   | BAT_ADC      | Mid-point of R8/R9 divider on VBAT               |
| GPIO 35   | BTN_PREV     | SW3 → GND, R3 100 kΩ ext pull-up to 3V3          |
| GPIO 36   | BTN_MENU     | SW6 → GND, R6 100 kΩ ext pull-up to 3V3          |
| GPIO 39   | BTN_VOLDN    | SW5 → GND, R5 100 kΩ ext pull-up to 3V3          |
| TXD0/RXD0 | UART0        | Test points / programming header                 |

> **Why external pull-ups on 34/35/36/39?** They are input-only pads on
> ESP32-WROOM-32 and lack internal pull-ups/pull-downs.

## PCM5102A I2S DAC (U2)

| Pin | Name   | Net / connection                                       |
| --- | ------ | ------------------------------------------------------ |
| 1   | DEMP   | GND (de-emphasis off)                                  |
| 2   | XSMT   | 3V3 via 10 kΩ (un-mute)                                |
| 3   | FMT    | GND (I²S mode)                                         |
| 4   | AGND   | GND                                                    |
| 5   | AVDD   | 3V3 via L1 ferrite, C9 4.7 µF + 100 nF decap           |
| 6   | VOUTL  | → R20 (10 Ω) → U3 INL (TPA6132A2)                      |
| 7   | CPVDD  | 3V3 via C10 4.7 µF                                     |
| 8   | VNEG   | C 1 µF flying cap (charge pump)                        |
| 9   | CAPP   | C 2.2 µF (charge pump)                                 |
| 10  | CAPM   | (paired with CAPP)                                     |
| 11  | VOUTR  | → R21 (10 Ω) → U3 INR                                  |
| 12  | AGND   | GND                                                    |
| 13  | BCK    | ESP32 GPIO 26                                          |
| 14  | DIN    | ESP32 GPIO 22                                          |
| 15  | LRCK   | ESP32 GPIO 25                                          |
| 16  | SCK    | GND (internal PLL mode — no master clock)              |
| 17  | DGND   | GND                                                    |
| 18  | DVDD   | 3V3 via L2 ferrite, 100 nF decap                       |
| 19  | FLT    | GND (normal latency filter)                            |
| 20  | OSR    | GND (×8 oversampling)                                  |

## TPA6132A2 headphone amp (U3)

| Pin | Name | Net / connection                                       |
| --- | ---- | ------------------------------------------------------ |
| 1   | INL  | from PCM5102A VOUTL via series 10 Ω                    |
| 2   | INR  | from PCM5102A VOUTR via series 10 Ω                    |
| 3   | GND  | GND                                                    |
| 4   | OUTL | J3 (3.5 mm jack) tip                                   |
| 5   | OUTR | J3 ring                                                |
| 6   | VSS  | GND (charge-pump negative)                             |
| 7   | C2N  | flying cap to C2P (1 µF)                               |
| 8   | C2P  | flying cap to C2N                                      |
| 9   | VCC  | 3V3 + 1 µF C11 + 100 nF                                |
| 10  | SD   | 3V3 via R17 47 kΩ (active-low shutdown)                |
| EP  | PAD  | GND (thermal)                                          |

> Cap-less ground-centred output — sleeve of J3 is **GND**, not a virtual
> ground; the chip itself swings around 0 V via internal charge pump.

## TP4056 charger (U4)

```
VBUS ── FB1 ── U4.5(VCC) ── C 10 µF
                U4.4(GND)
                U4.2(PROG) ── R1 2.4 k → GND   [Iset = 1200 / R1 ≈ 500 mA]
                U4.7(CHRG) ── R12 1 k → D1 → 3V3
                U4.6(STDBY)── R13 1 k → D2 → 3V3
                U4.1(TEMP) ── (NTC optional, otherwise GND)
                U4.3(CE)  ── 3V3
                U4.8(BAT) ── J4.+ (battery+)
```

## USB-C connector (J1) — 6-pin power-only

```
A4/B9 (VBUS) ─┬─ FB1 ─ TP4056 VCC
              └─ TVS1
A1,A12,B1,B12 (GND) ── GND
A5 (CC1) ── R10 5.1 k → GND      (UFP advertising default 5 V)
B5 (CC2) ── R11 5.1 k → GND
```

## microSD (J2)

| SD pin | Name | ESP32 pin |
| ------ | ---- | --------- |
| 1      | DAT2 | NC        |
| 2      | DAT3 | (CS) IO 5 |
| 3      | CMD  | (MOSI) 23 |
| 4      | VDD  | 3V3       |
| 5      | CLK  | (SCK) 18  |
| 6      | VSS  | GND       |
| 7      | DAT0 | (MISO) 19 |
| 8      | DAT1 | NC        |
| Card-detect → NC (use file-system mount as detect) | | |

## OLED header (J5)

| Pin | Net   |
| --- | ----- |
| 1   | GND   |
| 2   | VCC (3V3) |
| 3   | SCL → ESP32 GPIO 4  |
| 4   | SDA → ESP32 GPIO 21 |

## Buttons

All active-low to GND, each with a 100 nF de-bounce cap to GND on the
GPIO node:

| Switch | GPIO   | Pull-up                  |
| ------ | ------ | ------------------------ |
| SW1 PLAY  | 32  | internal                 |
| SW2 NEXT  | 33  | internal                 |
| SW3 PREV  | 35  | R3 100 kΩ external       |
| SW4 VOL+  | 14  | internal                 |
| SW5 VOL−  | 39  | R5 100 kΩ external       |
| SW6 MENU  | 36  | R6 100 kΩ external       |

## Power switch SW7

```
VBAT ── SW7.com
SW7.NC ── (open)
SW7.NO ── VBAT_SW ── U5.VIN
```

## Headphone jack J3 (PJ-313D, switched)

| Tag | Net / function                              |
| --- | ------------------------------------------- |
| 1 (T)   | OUTL from U3                            |
| 2 (R)   | OUTR from U3                            |
| 3 (S)   | GND                                     |
| 4 (sw)  | HP_DETECT (open when jack inserted)     |

`HP_DETECT` is pulled up to 3V3 by ESP32 internal pull-up; the switch
shorts it to OUTL when **not** inserted — when inserted, the switch
opens and the line floats high. (Firmware reads `LOW = inserted`; the
sense polarity matches a typical normally-closed jack switch.)

## Recommended PCB stack-up

2-layer FR-4, 1.6 mm, 1 oz copper, ENIG finish.

- **Top:** ESP32 module (centred), tact switches, OLED header, status
  LED. Keep the antenna corner of the ESP32 over a copper-free zone in
  both layers.
- **Bottom:** PCM5102A + TPA6132A2 (analog), TP4056, LDO, USB-C, microSD
  socket, headphone jack, slide switch, JST battery connector.
- **Power:** poured 3V3 / VBAT regions on bottom; a single solid GND
  pour on top under the ESP32 antenna kept clear.
