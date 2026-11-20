# ESP32-Organ-Pedalboard
Convert a traditional 32-note organ pedalboard into a USB MIDI controller using an ESP32-S3 and an 8×4 diode matrix. A low-cost DIY solution compatible with Hauptwerk, GrandOrgue and other MIDI software.

## Video
[Watch the project video](https://youtu.be/AH8gw19Z8Lw)

## Hardware and wiring

- A Freenove ESP32-S3 with native USB-C OTG, such as [this board](https://link.amazon/B0hbGeus3), and a USB data cable.
- A Viscount 32-note pedalboard with one diode per switch, arranged as 8 rows and 4 columns.

Connect the matrix directly to these **GPIO numbers**, not physical header pin numbers:

| Matrix line | ESP32-S3 GPIO |
| --- | --- |
| Row 0–7 | 4, 5, 6, 7, 15, 16, 17, 18 |
| Column 0–3 | 9, 10, 11, 12 |

On the author's Viscount PCB, the column connections were already present, but the parallel row connections had to be wired up. Inspect and trace your own PCB: layouts can differ. Disconnect the matrix from its original electronics and power before connecting it; the ESP32-S3 GPIOs are **3.3 V only**. Check your board's pinout for GPIO availability and conflicts before wiring.

The sketch defaults to diodes conducting from column to row (`COLUMN_TO_ROW = true`): each column is driven high in turn and rows are read with pull-downs. If the diodes conduct from row to column instead, set `COLUMN_TO_ROW` to `false`. The non-driven matrix lines are high impedance. Verify diode direction with a multimeter before connecting.

The default mapping is `36 + column * 8 + row`: column 0 covers MIDI notes 36–43, through column 3 covering 60–67. Wire or reorder the `rowPins` and `colPins` arrays to match the physical pedal order; adjust the mapping in the sketch if your PCB is arranged differently. The sketch sends note-on and note-off on MIDI channel 3, with velocity 100 and 15 ms debounce. These settings are near the top of the sketch.

## Flash with Arduino IDE

1. Install [Arduino IDE](https://www.arduino.cc/en/software/) and add `https://espressif.github.io/arduino-esp32/package_esp32_index.json` to **File → Preferences → Additional Boards Manager URLs**.
2. In **Boards Manager**, install **esp32 by Espressif Systems** version 3.3.7 (or a compatible newer version). The USB MIDI library is included in this package.
3. Open `ViscountPedalboard/ViscountPedalboard.ino`. Select your ESP32-S3 board (or **ESP32S3 Dev Module**) under **Tools → Board**, and select **USB Mode → USB-OTG (TinyUSB)**. Use the flash and PSRAM settings appropriate to your board.
4. Connect a data-capable USB cable to the board's native USB/OTG port, select its serial port under **Tools → Port**, and click **Upload**. If the port is unavailable or upload fails, enter the bootloader: hold **BOOT**, press and release **RESET**, then release **BOOT** and select the port again.
5. After upload, reset the board if needed and connect its native USB/OTG port to the computer running your MIDI software. Select the USB MIDI device (the MIDI interface is named **Viscount Pedalboard**) as an input, configure channel 3, and test every pedal for the expected note and release. A USB-to-serial-only port cannot provide the MIDI device.
