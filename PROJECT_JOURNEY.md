# ESP32 E-Paper Display Project - Development Journey

## Overview

This document chronicles the complete development journey of the ESP32 E-Paper Display project, from initial setup to a fully functional web-controlled display system with advanced features.

## Phase 1: Project Initialization (Getting Started)

### Initial Challenge
The project began with a simple request: "Can you help me set up a small ESP32 project using Cursor IDE?"

**Key Steps:**
- Installed PlatformIO Core manually when the IDE extension was stuck on initialization
- Set up basic ESP32 development environment
- Configured `platformio.ini` for ESP32 Dev Module
- Fixed initial compilation issues (LED_BUILTIN not defined for ESP32)

**Outcome:** Successfully created a working ESP32 project foundation with PlatformIO.

---

## Phase 2: E-Paper Display Integration

### Challenge: Display Not Working
The user had a 3.7" e-paper display that needed to be integrated.

**Issues Encountered:**
- Initial display model (`GxEPD2_370_GDEW037T1`) was incorrect
- Compiler errors about undefined display types

**Solutions:**
- Changed to `GxEPD2_370_GDEY037T03` based on compiler suggestions
- Added GxEPD2 library dependency
- Configured SPI pins correctly:
  - EPD_CS: GPIO 5
  - EPD_DC: GPIO 17
  - EPD_RST: GPIO 16
  - EPD_BUSY: GPIO 4

**Outcome:** Display successfully initialized and showing text.

---

## Phase 3: WiFi Access Point & Web Server

### Feature Request
User wanted: "Can the ESP32 host a WiFi server that if I connect to it on my phone I get a default web page that I can write into a message and press send to update the screen with that message."

**Implementation:**
- Created WiFi Access Point with SSID "ESP32-E-Paper" and password "12345678"
- Implemented WebServer on port 80
- Created HTML form with textarea for message input
- Added POST handler to receive and display messages
- Implemented word wrapping for text display

**Challenges:**
- Initial web page not loading (resolved through troubleshooting)
- Needed to ensure proper WiFi connection before accessing web interface

**Outcome:** Fully functional web interface allowing remote message updates via phone browser.

---

## Phase 4: QR Code Generation

### Initial Implementation
User requested QR codes to simplify WiFi connection and web access.

**First Attempt:**
- Tried `makuna/QRCode` library - installation failed
- Switched to `ricmoo/QRCode` library successfully
- Initial QR code was not scannable (too small, wrong format)

**Evolution of QR Code Features:**
1. **First version:** Single QR code for WiFi connection
2. **User feedback:** "I want it to be go to the web address code"
3. **Second version:** QR code encoding web URL (`http://IP_ADDRESS`)
4. **User feedback:** "Can you make it so that there are 2 qr codes, the left one is to join the network and the other is to join the website"
5. **Third version:** Two QR codes side-by-side
   - Left: WiFi join QR code (`WIFI:T:WPA;S:SSID;P:password;;`)
   - Right: Web address QR code (`http://IP_ADDRESS`)

**Refinements:**
- Removed IP address and WiFi name text display (user request)
- Added "WiFi" and "Web" labels above QR codes
- Increased QR code size (scale 7) and spacing (gap 60)
- Fixed WiFi QR code using version 3 instead of version 2 for more data capacity
- Made both QR codes the same display size (scaled differently based on module count)

**Outcome:** Two perfectly sized, scannable QR codes for easy WiFi and web access.

---

## Phase 5: Icelandic Character Support

### Challenge: Special Characters Not Displaying
User reported: "When 'Ó' is sent to the display it displays it as '?'"

**Root Cause:**
The FreeMonoBold9pt7b font doesn't include extended ASCII characters (characters above 127).

**Solution Implemented:**
1. **UTF-8 Conversion:** Created `handleUTF8()` function to convert UTF-8 encoded characters to extended ASCII
2. **Character Mapping:** Mapped Icelandic characters to their ISO-8859-1 equivalents:
   - ó, á, é, í, ú, ý, þ, ð (lowercase)
   - Ó, Á, É, Í, Ú, Ý, Þ, Ð (uppercase)
3. **Accent Rendering:** Created `drawCharWithAccent()` function that:
   - Draws the base character (e.g., 'O' for 'Ó')
   - Draws an accent mark above it using diagonal lines
   - Scales accent position based on font size

**Challenges:**
- Initial accent mark showed as "2 commas" instead of one - fixed by using single diagonal line
- Web form UTF-8 encoding - added proper charset headers and URL decoding
- Accent positioning needed adjustment for different font sizes

**Outcome:** Full support for Icelandic characters with proper accent rendering.

---

## Phase 6: Font Size Selection

### Feature Request
User wanted to change text size from the web interface.

**Implementation:**
- Added font size dropdown to web form (Small, Medium, Large, X-Large)
- Implemented 4 font sizes:
  - Small: 9pt (FreeMonoBold9pt7b)
  - Medium: 12pt (FreeMonoBold12pt7b) - default
  - Large: 18pt (FreeMonoBold18pt7b)
  - X-Large: 24pt (FreeMonoBold24pt7b)
- Dynamic font switching in `updateDisplay()` function
- Adjusted line heights and character widths for each size
- Scaled accent marks to match font size

**User Feedback:**
- "When the text increases the space between letters might need to increase also, on the x-large options the letters were crammed together"

**Solution:**
- Added character spacing that scales with font size:
  - Small: 0px spacing
  - Medium: 1px spacing
  - Large: 2px spacing
  - X-Large: 3px spacing
- Updated word wrapping logic to account for spacing

**Outcome:** Four font sizes with proper character spacing for optimal readability.

---

## Phase 7: Persistent Storage & Auto-Switching

### Feature Request
User asked: "Is there some storage on the ESP32? I want the QR codes to only display for a minute after boot then go back to the last sent message."

**Implementation:**
- Used ESP32's NVS (Non-Volatile Storage) via Preferences library
- Save functionality:
  - Message and font size saved to NVS when sent via web interface
  - Persists across reboots and power cycles
- Load functionality:
  - On boot, loads saved message and font size from NVS
  - Falls back to defaults if no saved data exists
- Auto-switching logic:
  - QR codes display for 1 minute (60,000ms) after boot
  - Automatically switches to saved message after timeout
  - Tracks boot time and elapsed time in main loop
  - Serial monitor shows countdown

**Outcome:** Messages persist across reboots, and display automatically switches from QR codes to content after 1 minute.

---

## Phase 8: Documentation & GitHub Repository

### Final Step
User requested: "Can you use GitHub CLI and create a GitHub repo for this project and push it. Please create a README.md file as well."

**Created:**
1. **README.md:** Comprehensive project documentation including:
   - Features overview
   - Hardware requirements
   - Installation instructions
   - Usage guide
   - Configuration options
   - Troubleshooting links
   - Project structure

2. **.gitignore:** Proper ignore rules for PlatformIO projects

3. **GitHub Repository:** 
   - Created public repository: `https://github.com/rufalo/esp32test`
   - Initial commit with all project files
   - Properly structured with documentation

**Outcome:** Professional, well-documented open-source project ready for sharing.

---

## Technical Challenges Overcome

### 1. PlatformIO Setup Issues
- **Problem:** Extension stuck on initialization
- **Solution:** Manual PlatformIO Core installation via pip

### 2. Display Model Compatibility
- **Problem:** Wrong display model causing compilation errors
- **Solution:** Identified correct model from compiler suggestions and library examples

### 3. QR Code Generation
- **Problem:** Library compatibility and scannability issues
- **Solution:** Multiple library attempts, proper version selection, size optimization

### 4. Character Encoding
- **Problem:** UTF-8 characters not displaying correctly
- **Solution:** Custom UTF-8 to extended ASCII conversion with accent rendering

### 5. Font Rendering
- **Problem:** Extended ASCII characters not in font
- **Solution:** Custom character rendering with accent marks

### 6. Web Form Encoding
- **Problem:** UTF-8 characters lost in form submission
- **Solution:** Proper charset headers and URL decoding

### 7. Display Spacing
- **Problem:** Large fonts cramped together
- **Solution:** Dynamic character spacing based on font size

---

## Key Features Implemented

✅ E-paper display integration (3.7" GDEY037T03)  
✅ WiFi Access Point mode  
✅ Web server with HTML interface  
✅ Two QR codes (WiFi join + Web access)  
✅ Icelandic character support with accent rendering  
✅ Four adjustable font sizes (9pt, 12pt, 18pt, 24pt)  
✅ Dynamic character spacing  
✅ Persistent message storage (NVS)  
✅ Auto-switching from QR codes to message after 1 minute  
✅ Word wrapping for long messages  
✅ Professional documentation  

---

## Development Statistics

- **Total Development Time:** Multiple sessions over extended period
- **Lines of Code:** ~574 lines in main.cpp
- **Libraries Used:** 5 (GxEPD2, QRCode, Adafruit GFX, WebServer, Preferences)
- **Features Added:** 8 major features
- **Issues Resolved:** 7+ technical challenges
- **Iterations:** Multiple refinements based on user feedback

---

## Lessons Learned

1. **User Feedback is Critical:** Each iteration improved based on specific user needs
2. **Font Limitations:** Bitmap fonts have character set limitations requiring custom rendering
3. **QR Code Optimization:** Size, version, and data format all matter for scannability
4. **ESP32 Storage:** NVS is perfect for small persistent data like settings
5. **Character Encoding:** UTF-8 handling requires careful conversion and rendering
6. **Spacing Matters:** Visual spacing needs to scale with font size for readability

---

## Future Enhancement Possibilities

- Additional font styles (Sans, Serif)
- More character sets (other languages)
- Image support
- Multiple message pages
- Scheduled message changes
- Remote API integration
- Battery level monitoring
- Display rotation options

---

## Conclusion

This project evolved from a simple "hello world" ESP32 setup to a sophisticated e-paper display system with web control, QR code integration, multi-language support, and persistent storage. The journey demonstrates iterative development, problem-solving, and feature refinement based on real-world usage and feedback.

The final product is a complete, production-ready solution for remote e-paper display control with professional documentation and open-source availability.

