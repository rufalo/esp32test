# ESP32 E-Paper Display with WiFi Web Interface

An ESP32 project that displays messages on an e-paper display with a web interface for remote updates. Features include QR code generation for WiFi connection and web access, support for Icelandic characters, adjustable font sizes, and persistent message storage.

## Features

- 📱 **E-Paper Display**: 3.7" e-paper display for low-power message display
- 🌐 **WiFi Access Point**: ESP32 creates its own WiFi network for easy connection
- 🔗 **Web Interface**: Simple web page to send messages to the display
- 📊 **QR Codes**: Two QR codes displayed on startup:
  - Left: Join WiFi network
  - Right: Access web interface
- 🇮🇸 **Icelandic Character Support**: Displays Icelandic characters (ó, á, é, í, ú, ý, etc.) with accent marks
- 📏 **Adjustable Font Sizes**: Choose from 4 font sizes (9pt, 12pt, 18pt, 24pt)
- 💾 **Persistent Storage**: Messages are saved to ESP32's NVS and persist across reboots
- ⏱️ **Auto-Switch**: QR codes display for 1 minute after boot, then automatically switch to the last saved message

## Hardware Requirements

- ESP32 development board
- 3.7" e-paper display (GDEY037T03 or compatible)
- Jumper wires for connections

## Wiring

See [WIRING.md](WIRING.md) for detailed wiring instructions.

**Pin Connections:**
- EPD_CS (Chip Select): GPIO 5
- EPD_DC (Data/Command): GPIO 17
- EPD_RST (Reset): GPIO 16
- EPD_BUSY (Busy): GPIO 4

## Software Requirements

- PlatformIO IDE (or VS Code with PlatformIO extension)
- ESP32 Arduino framework

## Installation

1. Clone this repository:
   ```bash
   git clone <repository-url>
   cd esp32test
   ```

2. Open the project in PlatformIO

3. Install dependencies (automatically handled by PlatformIO):
   - GxEPD2 library (v1.5.8+)
   - QRCode library (ricmoo/QRCode)
   - Adafruit GFX Library (for fonts)

4. Update `platformio.ini` with your COM port:
   ```ini
   upload_port = COM3  # Change to your port
   monitor_port = COM3
   ```

5. Upload the code to your ESP32

## Usage

1. **Power on the ESP32** - The display will show QR codes for 1 minute

2. **Connect to WiFi**:
   - Scan the left QR code with your phone, OR
   - Manually connect to WiFi network:
     - SSID: `ESP32-E-Paper`
     - Password: `12345678`

3. **Access Web Interface**:
   - Scan the right QR code, OR
   - Open browser and go to: `http://192.168.4.1`

4. **Send Messages**:
   - Enter your message in the text area
   - Select font size (Small, Medium, Large, X-Large)
   - Click "Send to Display"
   - The message will be saved and displayed immediately

5. **After 1 minute**: The display automatically switches from QR codes to the last saved message

## Configuration

### WiFi Credentials

Edit `src/main.cpp` to change WiFi credentials:

```cpp
const char* ssid = "ESP32-E-Paper";
const char* password = "12345678";
```

### QR Code Display Duration

Change the duration QR codes are displayed (default: 1 minute):

```cpp
const unsigned long QR_DISPLAY_DURATION = 60000; // milliseconds
```

## Supported Characters

- Standard ASCII characters
- Icelandic characters with accents:
  - Lowercase: ó, á, é, í, ú, ý, þ, ð
  - Uppercase: Ó, Á, É, Í, Ú, Ý, Þ, Ð

Accented characters are rendered by drawing the base character with an accent mark above it.

## Font Sizes

- **Small (9pt)**: Compact text, no character spacing
- **Medium (12pt)**: Default size, 1px spacing
- **Large (18pt)**: Larger text, 2px spacing
- **X-Large (24pt)**: Largest text, 3px spacing

## Troubleshooting

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for common issues and solutions.

### Common Issues

- **Web page not loading**: Check serial monitor for IP address and ensure you're connected to the ESP32 WiFi network
- **QR codes not scannable**: Ensure good lighting and hold phone steady
- **Icelandic characters show as "?"**: Characters are converted but may need font adjustment
- **Display not updating**: Check wiring connections and serial monitor for errors

## Project Structure

```
esp32test/
├── src/
│   └── main.cpp          # Main Arduino code
├── platformio.ini        # PlatformIO configuration
├── WIRING.md            # Wiring instructions
├── TROUBLESHOOTING.md   # Troubleshooting guide
└── README.md            # This file
```

## Libraries Used

- **GxEPD2**: E-paper display driver
- **QRCode**: QR code generation
- **Adafruit GFX**: Graphics and font support
- **Preferences**: Non-volatile storage (NVS)

## License

This project is open source. Feel free to modify and use as needed.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Author

Created for ESP32 e-paper display project with WiFi web interface.
