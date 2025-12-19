# E-Paper Display Wiring Guide

## Pin Connections for ESP32

Connect your 3.7" e-paper display to the ESP32 as follows:

| Display Pin | Wire Color | ESP32 Pin | Description |
|-------------|------------|-----------|-------------|
| VCC         | Red        | 3.3V      | Power (3.3V) |
| GND         | Black      | GND       | Ground |
| SDA         | Yellow     | GPIO 23   | SPI Data (MOSI) |
| SCL         | Green      | GPIO 18   | SPI Clock (SCK) |
| CS          | Blue       | GPIO 5    | Chip Select |
| D/C         | White      | GPIO 17   | Data/Command |
| RES         | Orange     | GPIO 16   | Reset |
| Busy        | Purple     | GPIO 4    | Busy (optional) |

## Notes

- **Power**: Make sure to use 3.3V, not 5V!
- **BUSY pin**: If your display doesn't have a BUSY pin, you can set it to -1 in the code
- **SPI pins**: ESP32 has multiple SPI buses. The code uses VSPI (default)
- **Pin changes**: If you need different pins, edit the `#define` statements at the top of `src/main.cpp`

## Display Model

The code is configured for `GxEPD2_370_GDEY037T03` (3.7" 240x416 pixels).

**If your display doesn't work**, you may need to change the display model in `src/main.cpp`. Common alternatives for 3.7" displays:
- `GxEPD2_370_TC1` (3.7" alternative)
- `GxEPD2_420_GDEW042T2` (4.2" 400x300)
- `GxEPD2_290_GDEW029T5` (2.9" 128x296)

To change the display model, edit line 17 in `src/main.cpp`:
```cpp
GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> display(...);
```

Check your display's datasheet or label for the exact model number. The WeAct Studio 3.7" display typically uses GDEY037T03 or similar.

