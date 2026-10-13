# WLED power support for M5StickC Plus (AXP192)

An experimental, small [WLED](https://github.com/wled/WLED) usermod to enable the external 5V output on the **M5StickC Plus 1.1** by initialising its AXP192 power-management IC. Standard generic ESP32 WLED builds do not necessarily turn on this output.

**Status: one-device proof of concept.** The code was compiled and tested with WLED **0.16.0.1** on an **M5StickC Plus 1.1**, with a short, 20-pixel, 5V three-wire addressable LED strip. The LEDs illuminated after flashing the custom firmware. This is **not** a verified power-capacity rating or long-term reliability test. Other M5Stick models, including **M5StickC Plus2**, are **not tested or supported** by this code.

## What it does

At WLED startup the usermod talks to the AXP192 at I²C address `0x34` via GPIO21 (SDA) and GPIO22 (SCL), and sets these bits without clearing other register bits:

- Register `0x12`, mask `0x40` — enable EXTEN.
- Register `0x10`, mask `0x04` — enable the external 5V boost output.

This follows the relevant register operations in [M5Stack's M5StickC Plus AXP192 implementation](https://github.com/m5stack/M5StickC-Plus/blob/master/src/AXP192.cpp). It does **not** initialise the screen, buttons, or any other M5-specific functions.

## Wiring used in the test

With power **disconnected** before wiring:

| M5StickC Plus 1.1 | Addressable strip input |
| --- | --- |
| `G26` | `DIN` (data input) |
| `GND` | `GND` |
| **`5V OUT`** (the correctly labelled output pin) | `+5V` |

- Use the **input** end of the strip, following the strip's data-direction arrows.
- Verify pin labels against **your exact board revision**; do not confuse `5V OUT` and `5V IN`, or connect the LEDs to `3V3` / `BAT`.
- The M5Stick was powered by USB-C for the test.

### Electrical safety

**The M5Stick 5V output is not a general-purpose LED-strip power supply.** The maximum safe external load was **not measured or established** in this experiment. Do **not** assume that 20 pixels, or a WLED current-limit setting, makes the power wiring intrinsically safe. WLED's limiter is an estimate, not over-current protection. For sustained use, higher brightness, or more pixels, use a suitably rated external 5V supply, connect grounds, and consider proper 3.3V-to-5V data-level shifting. Incorrect 5V/GND polarity may damage the device or LEDs.

## Build with WLED 0.16.0.1 (tested method)

Install [PlatformIO](https://platformio.org/) (the VS Code extension is convenient). In a terminal:

```sh
git clone --depth 1 --branch v16.0.1 https://github.com/wled/WLED.git WLED
```

1. Copy **this repository's** `M5StickC_Plus_Power.cpp` and `library.json` into `WLED/usermods/M5StickC_Plus_Power/`.
2. Copy the sections from [`platformio_override.ini.example`](platformio_override.ini.example) into `WLED/platformio_override.ini` (create it if necessary).
3. In the WLED project, run `pio run -e m5stickc_plus`, or use the matching PlatformIO Build task.
4. Back up WLED settings/presets before updating. Upload the resulting `.pio/build/m5stickc_plus/firmware.bin` via **WLED → Config → Security & Updates → Update WLED** (or flash by USB if required).
5. After it reboots, verify the WLED web interface still works **before** reconnecting the strip.

For the 20-LED proof of concept, WLED **Config → LED Preferences** used:

- LED type: `WS281x RGB`
- GPIO: `26`
- Length: `20`
- Automatic brightness limiter enabled, configured at `250 mA`
- Global brightness factor at `10%`

Those settings are **test precautions, not a certification of safe output current**. Start with a dim colour and monitor for resets, overheating or other unexpected behaviour.

For reusable/out-of-tree installs, see [WLED custom features](https://kno.wled.ge/advanced/custom-features/) and the [official usermod template](https://github.com/wled/wled-usermod-example). That workflow uses a `symlink://` entry in `custom_usermods` referencing your local clone.

## Troubleshooting

- **WLED web UI works, no LEDs:** verify that this *custom* firmware was flashed, then check `5V OUT` and common ground (ideally with a multimeter). Also check correct `DIN` end and GPIO26.
- **WLED can see Wi-Fi but cannot join it:** check 2.4GHz access and AP security. In the original test, BT Whole Home mesh required its temporary **2.4 GHz Setup Mode** to complete initial pairing; that is network-specific, not a required WLED modification.
- **Blank M5Stick screen:** this usermod does not drive the integrated display.

## Tested / untested

| Item | Result |
| --- | --- |
| M5StickC Plus 1.1 + WLED 0.16.0.1 | Compiled and boots; LEDs lit |
| GPIO26, 20-pixel 5V addressable strip | Worked at low brightness |
| Cold-start reliability, long-term operation | Not yet documented |
| Maximum safe LED load from onboard 5V output | Unknown — not tested |
| Original M5StickC / M5StickC Plus2 | Not tested |
| Other WLED releases | Not tested |

Contributions and independent test reports are welcome. Please include the exact hardware revision, WLED version, LED type, power source, and behaviour.

## Licence

[MIT](LICENSE). This is an independent community experiment, not an official WLED or M5Stack firmware release.
