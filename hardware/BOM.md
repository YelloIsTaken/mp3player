# Bill of Materials

All resistors 0402 1 % unless noted. Capacitors X7R/X5R 0402 unless noted.
Tantalums and electrolytics avoided to keep height ≤ 2.0 mm.

| Ref           | Qty | Part                                 | Package        | Notes                                            |
| ------------- | --- | ------------------------------------ | -------------- | ------------------------------------------------ |
| **Active**    |     |                                      |                |                                                  |
| U1            | 1   | ESP32-WROOM-32E (4 MB)               | SMD module     | Main MCU, WiFi/BT antenna onboard                |
| U2            | 1   | PCM5102A                             | TSSOP-20       | I2S stereo DAC, 32-bit / 384 kHz                 |
| U3            | 1   | TPA6132A2                            | VSON-10 (3×3)  | Cap-less stereo HP amp, 25 mW into 32 Ω          |
| U4            | 1   | TP4056                               | SOP-8          | LiPo charger, set Iset = 500 mA via 2.4 kΩ       |
| U5            | 1   | AP2112K-3.3                          | SOT-23-5       | 3.3 V LDO, 600 mA, 55 µA Iq                      |
| U6            | 1   | DMG2305UX or AO3401 P-MOSFET         | SOT-23         | Reverse battery / power-path                     |
| **Display**   |     |                                      |                |                                                  |
| OLED          | 1   | SSD1306 128×64 OLED 0.96"            | 4-pin module   | I2C @ 0x3C; 27.0 × 27.8 × 1.5 mm panel           |
| **Connectors**|     |                                      |                |                                                  |
| J1            | 1   | USB-C 6-pin power-only               | TYPE-C-31-M-12 | VBUS/GND/CC1/CC2 only                            |
| J2            | 1   | microSD push-pull socket             | DM3AT-SF-PEJM5 | SPI mode                                         |
| J3            | 1   | 3.5 mm stereo jack PJ-313D           | TH SMD-hybrid  | Switched (HP-detect)                             |
| J4            | 1   | JST-PH 2-pin battery connector       | S2B-PH-K       | Or solder pads                                   |
| J5            | 1   | 4-pin 0.1" header (OLED)             | THT            | VCC/GND/SCL/SDA                                  |
| **Switches**  |     |                                      |                |                                                  |
| SW1..SW6      | 6   | Tactile switch 4×4×1.5 mm SMD        | SKQG / similar | Caps printed                                     |
| SW7           | 1   | Slide switch SS-12D00 SPDT           | SMD            | Power on/off                                     |
| **Battery**   |     |                                      |                |                                                  |
| BAT1          | 1   | 502030 LiPo 500 mAh                  | wired JST-PH   | 5 × 20 × 30 mm                                   |
| **Indicators**|     |                                      |                |                                                  |
| D1            | 1   | LED red 0603 (CHG)                   | 0603           | TP4056 CHRG pin                                  |
| D2            | 1   | LED green 0603 (STDBY)               | 0603           | TP4056 STDBY pin                                 |
| D3            | 1   | LED blue/white 0603 (status)         | 0603           | ESP32 GPIO 2                                     |
| **Passives**  |     |                                      |                |                                                  |
| C1, C2        | 2   | 10 µF / 10 V X5R                     | 0603           | VBAT/3V3 bulk                                    |
| C3..C8        | 6   | 100 nF / 16 V X7R                    | 0402           | Decoupling on each IC VCC                        |
| C9, C10       | 2   | 4.7 µF / 10 V                        | 0402           | PCM5102A AVDD/CPVDD                              |
| C11           | 1   | 1 µF / 10 V                          | 0402           | TPA6132A2 charge pump                            |
| C12, C13      | 2   | 1 µF / 10 V                          | 0402           | LDO in/out                                       |
| C14, C15      | 2   | 22 pF                                | 0402           | USB-C CC line bypass (optional)                  |
| R1            | 1   | 2.4 kΩ                               | 0402           | TP4056 ISET → 500 mA                             |
| R2..R7        | 6   | 100 kΩ                               | 0402           | External pull-ups for buttons on IO34/35/36/39   |
| R8, R9        | 2   | 100 kΩ                               | 0402           | Battery sense divider (50 % attenuation)         |
| R10, R11      | 2   | 5.1 kΩ                               | 0402           | USB-C CC1/CC2 pull-downs (UFP power-only)        |
| R12, R13      | 2   | 1 kΩ                                 | 0402           | LED current limit (CHG/STDBY)                    |
| R14           | 1   | 1 kΩ                                 | 0402           | Status LED current limit                         |
| R15           | 1   | 10 kΩ                                | 0402           | EN pull-up on ESP32                              |
| R16           | 1   | 10 kΩ                                | 0402           | LDO EN pull-up                                   |
| R17           | 1   | 47 kΩ                                | 0402           | TPA6132A2 SD pull-up                             |
| R18, R19      | 2   | 4.7 kΩ                               | 0402           | I2C pull-ups (SDA/SCL)                           |
| L1, L2        | 2   | Ferrite bead 600 Ω @ 100 MHz         | 0402           | AVDD/DVDD isolation on PCM5102A                  |
| FB1           | 1   | Ferrite bead 600 Ω @ 100 MHz         | 0402           | USB VBUS                                         |
| TVS1          | 1   | SRV05-4 / USBLC6-2SC6                | SOT-23-6       | USB-C VBUS+CC ESD                                |

## Estimated cost

≈ **USD 12–16** in single quantities (excluding PCB and 3D-printed case).

Bulk of cost: ESP32 module (~$3), PCM5102A (~$1.5), USB-C connector,
SSD1306 module, 500 mAh LiPo (~$3).
