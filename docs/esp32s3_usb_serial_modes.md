# ESP32-S3 USB Mode & Serial Configuration

Two independent build flags control USB and serial behaviour:

| Flag | Values | Controls |
|---|---|---|
| `ARDUINO_USB_MODE` | `0` = USB OTG, `1` = USB Serial/JTAG (**default**) | Which USB hardware block is active |
| `ARDUINO_USB_CDC_ON_BOOT` | `0` = UART (**default**), `1` = USB CDC | What `Serial` is aliased to at boot |

---

## Serial object mapping

| Object | Always maps to | Notes |
|---|---|---|
| `Serial` | Depends on flags — see matrix below | Alias; target changes at compile time |
| `Serial0` | UART0 — GPIO43 (TX), GPIO44 (RX) | Always UART0, regardless of USB mode |
| `Serial1` | UART1 — pins configurable via `begin()` | Always UART1, unaffected by USB flags |
| `Serial2` | UART2 — pins configurable via `begin()` | Always UART2, unaffected by USB flags |

When `CDC_ON_BOOT=1`, `Serial` is remapped to USB CDC and **UART0 is only reachable via `Serial0`**.  
When `CDC_ON_BOOT=0`, `Serial` and `Serial0` both refer to the same UART0 hardware.

---

## Full combination matrix

| `USB_MODE` | `CDC_ON_BOOT` | VID:PID | `Serial` maps to | `Serial0` | `Serial1`/`Serial2` | JTAG debug | esptool auto-reset | `esp-builtin` upload |
|---|---|---|---|---|---|---|---|---|
| **0** (OTG) | **0** | `0x303A:0x0002` | UART0 (= `Serial0`) | UART0 | UART1 / UART2 | No | Software: firmware must handle `!DTR+RTS` or 1200 bps | No |
| **0** (OTG) | **1** | `0x303A:0x0002` | USB OTG CDC (`USBCDC`) | UART0 | UART1 / UART2 | No | Software: firmware must handle `!DTR+RTS` or 1200 bps | No |
| **1** (HW JTAG) | **0** | `0x303A:0x1001` | UART0 (= `Serial0`) | UART0 | UART1 / UART2 | Yes | Hardware: RTS → CHIP\_PU, DTR → GPIO0 | Yes |
| **1** (HW JTAG) | **1** | `0x303A:0x1001` | USB JTAG/Serial CDC (`HWCDC`) | UART0 | UART1 / UART2 | Yes | Hardware: RTS → CHIP\_PU, DTR → GPIO0 | Yes |

---

## Notes per mode

### `USB_MODE=0` + `CDC_ON_BOOT=0` — pure UART development
- No USB serial; debug requires a USB-UART adapter on GPIO43/44.
- USB OTG is available for application use (HID, MSC, etc.) but `Serial` is not on it.
- Flashing: UART only, or manual BOOT+RESET.

### `USB_MODE=0` + `CDC_ON_BOOT=1` — OTG CDC debug
- `Serial` → software USB CDC over OTG peripheral.
- `Serial.print()` **blocks** until a host opens the port (DTR asserted).
- Auto-reset into bootloader requires the firmware to respond to the line-state trigger (`!DTR + RTS`) or a 1200 bps baud-rate change — both are handled by the arduino-esp32 `USBCDC` class but depend on the firmware running correctly.
- No JTAG; cannot use `esp-builtin` for upload.
- UART0 still accessible via `Serial0` for a second debug channel or GPS/peripheral.

### `USB_MODE=1` + `CDC_ON_BOOT=0` — JTAG-only USB, UART debug
- USB exposes JTAG + a CDC port, but `Serial` goes to UART0.
- The JTAG/CDC composite device (`0x1001`) is always present, even in the ROM bootloader.
- `esp-builtin` (OpenOCD) works for upload without entering bootloader mode.
- Useful when GPIO43/44 are wired to a host UART for logging.

### `USB_MODE=1` + `CDC_ON_BOOT=1` — JTAG + USB serial debug ← **recommended for development**
- `Serial` → hardware JTAG/Serial CDC (`HWCDC`); single USB cable does everything.
- Auto-reset via RTS/DTR is handled in **silicon** — no firmware cooperation needed.
- `esp-builtin` uploads over JTAG without touching the CDC serial port; monitor can stay open while flashing.
- UART0 still reachable via `Serial0` for a peripheral (e.g. GPS on this project).
- This is the effective mode when `ARDUINO_USB_MODE` is left at its default.

---

## PlatformIO upload settings

### `upload_protocol`

Controls which tool and transport PlatformIO uses to write the flash.

| Value | Tool | Transport | Needs bootloader mode? | Works with USB_MODE |
|---|---|---|---|---|
| `esptool` | esptool.py | Serial port (COM/ttyUSB) | **Yes** — device must be in ROM download mode | 0 or 1 (via COM port) |
| `esp-builtin` | OpenOCD | USB JTAG interface | **No** — programs live over JTAG | 1 only (`0x1001` must be present) |
| `esp-prog` | OpenOCD | External ESP-Prog JTAG probe | No | Any (external hardware) |
| `uart` | esptool.py | Hardware UART (USB-UART adapter) | Yes | Any (independent of USB) |
| `dfu` | dfu-util | USB DFU (Device Firmware Upgrade) | Yes — device must expose DFU descriptor | 0 only (OTG, requires DFU class in firmware) |

**For this board** the effective choice is between `esptool` (needs manual BOOT+RESET or working auto-reset) and `esp-builtin` (no button press, monitor can stay open).

---

### `upload_resetmethod`

Only applies when `upload_protocol = esptool`. Tells esptool how to put the device into ROM download mode before flashing.

| Value | How it resets | Works with | Requires |
|---|---|---|---|
| `default_reset` | Sequences DTR→low then toggles RTS via the COM port | USB_MODE=1 (hardware maps RTS→CHIP_PU, DTR→GPIO0) or any board with a USB-UART chip (e.g. CP2102) | COM port must not be held open by another process |
| `usb` | Sends a USB control transfer directly to the device (bypasses COM port driver) | USB_MODE=1 USB JTAG/Serial hardware only | WinUSB driver on Windows (not the CDC/COM driver); `libusb` on Linux/Mac |
| `no_reset` | Does not reset — assumes device is already in bootloader | Any | Device must be in download mode before upload starts |
| `ck` | Legacy: pulses RTS only (no DTR) | Old Arduino-style boards with CH340/FTDI | External USB-UART chip wired to EN pin |
| `nodemcu` | Legacy: inverted DTR/RTS logic | NodeMCU v1/v2 boards | External USB-UART chip with specific wiring |

**Reset sequence for `default_reset`** (how esptool gets the chip into download mode):
```
DTR=0  →  GPIO0 pulled low (BOOT pin)
RTS=1  →  CHIP_PU pulled low (chip in reset)
RTS=0  →  CHIP_PU released (chip starts with GPIO0=0 → enters download mode)
DTR=1  →  GPIO0 released
```
On USB_MODE=1 hardware, the USB JTAG/Serial controller maps these directly to chip pins in silicon — no firmware cooperation needed. On USB OTG (USB_MODE=0), these are software callbacks in the `USBCDC` class and only work if the firmware is running.

---

### `board_upload.wait_for_upload_port`

| Value | Behaviour |
|---|---|
| `false` (default) | esptool uses the port as-is; if the port disappears during reset it will fail |
| `true` | PlatformIO snapshots COM ports before triggering reset, then waits up to ~10 s for a **new** port to appear, and passes that new port to esptool |

**When you need it:** whenever the device re-enumerates on a **different COM number** after reset — e.g. when the application CDC port (`0x0002`) disappears and the ROM bootloader CDC port (`0x0001` or a new number) appears. Not needed for `esp-builtin` (JTAG never changes port) or for `USB_MODE=1` where both modes use the same `0x1001` device on the same COM number.

---

## This project (`waveshare_esp32_s3_touch_lcd_4`)

`boards/waveshare_esp32_s3_touch_lcd_4.json` declares:
```json
"-DARDUINO_USB_MODE=0",
"-DARDUINO_USB_CDC_ON_BOOT=1"
```
However the device enumerates as `0x303A:0x1001` (USB JTAG/Serial), indicating those flags are **not taking effect** and the chip is running with the hardware defaults (`USB_MODE=1`). The current `platformio.ini` matches this reality:

```ini
upload_protocol = esp-builtin   ; OpenOCD over JTAG — no bootloader dance needed
```

GPS uses `Serial2` (RX GPIO13, TX GPIO14) and is unaffected by any of the above.
