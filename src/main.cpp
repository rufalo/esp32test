#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <qrcode.h>
#include <Preferences.h>

// Pin definitions for ESP32
#define EPD_CS      5   // Chip Select
#define EPD_DC      17  // Data/Command
#define EPD_RST     16  // Reset
#define EPD_BUSY    4   // Busy

// WiFi Access Point credentials
const char* ssid = "ESP32-E-Paper";
const char* password = "12345678";  // Change this if you want

// Web server on port 80
WebServer server(80);

// Store the message to display
String displayMessage = "Hello World!";

// Store the font size (1=small, 2=medium, 3=large, 4=xlarge)
int fontSize = 2; // Default to medium (12pt)

// E-paper display
GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> display(GxEPD2_370_GDEY037T03(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

// Preferences for storing data in NVS (Non-Volatile Storage)
Preferences preferences;

// Track boot time and QR code display duration
unsigned long bootTime = 0;
const unsigned long QR_DISPLAY_DURATION = 60000; // 1 minute in milliseconds
bool showingQRCode = true;

// Function to draw a real QR code on the display
void drawQRCode(QRCode qrcode, int x, int y, int scale) {
  // Draw the QR code module by module
  for (uint8_t qy = 0; qy < qrcode.size; qy++) {
    for (uint8_t qx = 0; qx < qrcode.size; qx++) {
      if (qrcode_getModule(&qrcode, qx, qy)) {
        display.fillRect(x + qx * scale, y + qy * scale, scale, scale, GxEPD_BLACK);
      }
    }
  }
}

// Function to draw a character with accent support (forward declaration)
int drawCharWithAccent(char c, int x, int y);

// Function to display two QR codes side by side
void displayQRCode() {
  Serial.println("Generating QR codes...");
  
  // Get the IP address that ESP32 assigned itself
  IPAddress IP = WiFi.softAPIP();
  
  // Create WiFi QR code string (left QR code)
  String wifiQRData = "WIFI:T:WPA;S:" + String(ssid) + ";P:" + String(password) + ";;";
  
  // Create web address QR code string (right QR code)
  String webQRData = "http://" + IP.toString();
  
  Serial.println("WiFi QR Data: " + wifiQRData);
  Serial.println("Web QR Data: " + webQRData);
  
  // Generate WiFi QR code (use version 3 for more data capacity)
  QRCode wifiQR;
  uint8_t wifiQRDataBuffer[qrcode_getBufferSize(3)];
  int wifiResult = qrcode_initText(&wifiQR, wifiQRDataBuffer, 3, 0, wifiQRData.c_str());
  
  if (wifiResult != 0) {
    Serial.println("WiFi QR code generation failed, trying version 2...");
    // Fallback to version 2 if version 3 fails
    uint8_t wifiQRDataBuffer2[qrcode_getBufferSize(2)];
    wifiResult = qrcode_initText(&wifiQR, wifiQRDataBuffer2, 2, 0, wifiQRData.c_str());
    if (wifiResult != 0) {
      Serial.println("WiFi QR code generation failed!");
      return;
    }
  }
  
  // Generate Web QR code
  QRCode webQR;
  uint8_t webQRDataBuffer[qrcode_getBufferSize(2)];
  int webResult = qrcode_initText(&webQR, webQRDataBuffer, 2, 0, webQRData.c_str());
  
  if (webResult != 0) {
    Serial.println("Web QR code generation failed!");
    return;
  }
  
  Serial.println("Both QR codes generated successfully!");
  Serial.println("WiFi QR size: " + String(wifiQR.size) + " modules");
  Serial.println("Web QR size: " + String(webQR.size) + " modules");
  
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    
    // Target display size for both QR codes (same pixel size)
    int targetDisplaySize = 175;  // Target size in pixels (25 modules * 7 scale)
    
    // Calculate scale for each QR code to achieve same display size
    int wifiScale = targetDisplaySize / wifiQR.size;
    int webScale = targetDisplaySize / webQR.size;
    
    // Actual display sizes
    int wifiDisplaySize = wifiQR.size * wifiScale;
    int webDisplaySize = webQR.size * webScale;
    
    // Use the larger of the two for spacing calculations
    int maxDisplaySize = (wifiDisplaySize > webDisplaySize) ? wifiDisplaySize : webDisplaySize;
    
    // Calculate spacing - two QR codes with more gap between them
    int gap = 60;  // Larger gap between QR codes
    int totalWidth = (maxDisplaySize * 2) + gap;
    int startX = (display.width() - totalWidth) / 2;
    int labelY = 10;  // Y position for labels
    int qrY = labelY + 20;  // Y position for QR codes (below labels)
    
    // Center each QR code within its allocated space
    int leftQRX = startX + (maxDisplaySize - wifiDisplaySize) / 2;
    int rightQRX = startX + maxDisplaySize + gap + (maxDisplaySize - webDisplaySize) / 2;
    
    // Set font for labels
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    
    // Draw left label (centered above QR code)
    int leftLabelX = startX + (maxDisplaySize / 2) - 15;  // Approximate center
    display.setCursor(leftLabelX, labelY);
    display.print("WiFi");
    
    // Draw left QR code (WiFi join)
    drawQRCode(wifiQR, leftQRX, qrY, wifiScale);
    
    // Draw right label (centered above QR code)
    int rightLabelX = startX + maxDisplaySize + gap + (maxDisplaySize / 2) - 10;  // Approximate center
    display.setCursor(rightLabelX, labelY);
    display.print("Web");
    
    // Draw right QR code (Web address)
    drawQRCode(webQR, rightQRX, qrY, webScale);
    
  }
  while (display.nextPage());
  
  Serial.println("Both QR codes displayed!");
  Serial.println("Left: WiFi join, Right: Web address");
}

// Function to handle UTF-8 characters (including Icelandic)
// Converts UTF-8 encoded characters to displayable format
// Note: FreeMono fonts may not support all extended ASCII characters
String handleUTF8(String text) {
  String result = "";
  Serial.print("Original UTF-8: ");
  for (int i = 0; i < text.length(); i++) {
    Serial.print("0x");
    if (text[i] < 16) Serial.print("0");
    Serial.print((unsigned char)text[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  for (int i = 0; i < text.length(); i++) {
    unsigned char c = text[i];
    // Check if this is a UTF-8 multi-byte character
    if ((c & 0x80) == 0) {
      // Single byte ASCII character
      result += (char)c;
    } else if ((c & 0xE0) == 0xC0) {
      // Two-byte UTF-8 character (like ó, á, é, etc.)
      if (i + 1 < text.length()) {
        unsigned char c2 = text[i + 1];
        // Map common Icelandic characters to Latin-1/ISO-8859-1 codes
        // These match the UTF-8 to ISO-8859-1 conversion
        if (c == 0xC3) {
          switch (c2) {
            case 0xB3: result += (char)0xF3; break; // ó -> 0xF3 (243)
            case 0xA1: result += (char)0xE1; break; // á -> 0xE1 (225)
            case 0xA9: result += (char)0xE9; break; // é -> 0xE9 (233)
            case 0xAD: result += (char)0xED; break; // í -> 0xED (237)
            case 0xBA: result += (char)0xFA; break; // ú -> 0xFA (250)
            case 0xBD: result += (char)0xFD; break; // ý -> 0xFD (253)
            case 0xBE: result += (char)0xFE; break; // þ -> 0xFE (254)
            case 0xB0: result += (char)0xF0; break; // ð -> 0xF0 (240)
            case 0x93: result += (char)0xD3; break; // Ó -> 0xD3 (211)
            case 0x81: result += (char)0xC1; break; // Á -> 0xC1 (193)
            case 0x89: result += (char)0xC9; break; // É -> 0xC9 (201)
            case 0x8D: result += (char)0xCD; break; // Í -> 0xCD (205)
            case 0x9A: result += (char)0xDA; break; // Ú -> 0xDA (218)
            case 0x9D: result += (char)0xDD; break; // Ý -> 0xDD (221)
            case 0x9E: result += (char)0xDE; break; // Þ -> 0xDE (222)
            case 0x90: result += (char)0xD0; break; // Ð -> 0xD0 (208)
            default: 
              Serial.print("Unknown UTF-8: 0xC3 0x");
              Serial.println(c2, HEX);
              result += '?'; 
              break;
          }
          i++; // Skip the next byte as we've processed it
        } else {
          result += '?'; // Unknown UTF-8 sequence
        }
      }
    } else {
      // Other UTF-8 sequences - skip or replace with ?
      result += '?';
    }
  }
  
  Serial.print("Converted: ");
  Serial.println(result);
  return result;
}

// Function to draw a character with accent support
// Returns the width of the character drawn
int drawCharWithAccent(char c, int x, int y) {
  // Calculate character width based on font size
  int charWidth;
  int accentOffset;
  switch(fontSize) {
    case 1: charWidth = 9; accentOffset = 12; break;
    case 2: charWidth = 11; accentOffset = 14; break;
    case 3: charWidth = 16; accentOffset = 20; break;
    case 4: charWidth = 21; accentOffset = 26; break;
    default: charWidth = 11; accentOffset = 14; break;
  }
  char baseChar = c;
  bool hasAccent = false;
  
  Serial.print("drawCharWithAccent: char=0x");
  Serial.print((unsigned char)c, HEX);
  Serial.print(" (");
  Serial.print((unsigned char)c);
  Serial.print(")");
  
  // Map extended ASCII Icelandic characters to base characters + accent
  if ((unsigned char)c >= 0x80) {
    switch ((unsigned char)c) {
      case 0xF3: baseChar = 'o'; hasAccent = true; break; // ó
      case 0xE1: baseChar = 'a'; hasAccent = true; break; // á
      case 0xE9: baseChar = 'e'; hasAccent = true; break; // é
      case 0xED: baseChar = 'i'; hasAccent = true; break; // í
      case 0xFA: baseChar = 'u'; hasAccent = true; break; // ú
      case 0xFD: baseChar = 'y'; hasAccent = true; break; // ý
      case 0xD3: baseChar = 'O'; hasAccent = true; break; // Ó
      case 0xC1: baseChar = 'A'; hasAccent = true; break; // Á
      case 0xC9: baseChar = 'E'; hasAccent = true; break; // É
      case 0xCD: baseChar = 'I'; hasAccent = true; break; // Í
      case 0xDA: baseChar = 'U'; hasAccent = true; break; // Ú
      case 0xDD: baseChar = 'Y'; hasAccent = true; break; // Ý
      case 0xFE: baseChar = 'p'; hasAccent = false; break; // þ
      case 0xF0: baseChar = 'd'; hasAccent = false; break; // ð
      case 0xDE: baseChar = 'P'; hasAccent = false; break; // Þ
      case 0xD0: baseChar = 'D'; hasAccent = false; break; // Ð
      default: 
        baseChar = '?'; 
        Serial.print(" -> UNKNOWN");
        break;
    }
    Serial.print(" -> baseChar='");
    Serial.print(baseChar);
    Serial.print("' hasAccent=");
    Serial.print(hasAccent);
  }
  Serial.println();
  
  // Draw the base character
  display.setCursor(x, y);
  display.print(baseChar);
  
  // Draw accent mark if needed (acute accent - single diagonal line)
  if (hasAccent) {
    // Draw a single diagonal line for the acute accent (from bottom-left to top-right)
    // Adjust accent position based on font size
    int accentX1, accentY1, accentX2, accentY2;
    switch(fontSize) {
      case 1: accentX1 = x + 2; accentY1 = y - 11; accentX2 = x + 6; accentY2 = y - 13; break;
      case 2: accentX1 = x + 2; accentY1 = y - 13; accentX2 = x + 7; accentY2 = y - 16; break;
      case 3: accentX1 = x + 3; accentY1 = y - 18; accentX2 = x + 10; accentY2 = y - 22; break;
      case 4: accentX1 = x + 4; accentY1 = y - 24; accentX2 = x + 13; accentY2 = y - 29; break;
      default: accentX1 = x + 2; accentY1 = y - 13; accentX2 = x + 7; accentY2 = y - 16; break;
    }
    display.drawLine(accentX1, accentY1, accentX2, accentY2, GxEPD_BLACK);
  }
  
  return charWidth;
}

// Function to update the e-paper display with text message
void updateDisplay(String message) {
  Serial.println("Updating display with: " + message);
  Serial.print("Font size: ");
  Serial.println(fontSize);
  
  // Handle UTF-8 characters (including Icelandic)
  String processedMessage = handleUTF8(message);
  
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    
    // Set font based on fontSize (1=9pt, 2=12pt, 3=18pt, 4=24pt)
    switch(fontSize) {
      case 1: display.setFont(&FreeMonoBold9pt7b); break;
      case 2: display.setFont(&FreeMonoBold12pt7b); break;
      case 3: display.setFont(&FreeMonoBold18pt7b); break;
      case 4: display.setFont(&FreeMonoBold24pt7b); break;
      default: display.setFont(&FreeMonoBold12pt7b); break;
    }
    display.setTextColor(GxEPD_BLACK);
    
    // Calculate line height and character width based on font size
    // Include spacing between characters in the width calculation
    int lineHeight, charWidth, charSpacing, startY;
    switch(fontSize) {
      case 1: lineHeight = 20; charWidth = 9; charSpacing = 0; startY = 20; break;
      case 2: lineHeight = 25; charWidth = 11; charSpacing = 1; startY = 25; break;
      case 3: lineHeight = 35; charWidth = 16; charSpacing = 2; startY = 30; break;
      case 4: lineHeight = 45; charWidth = 21; charSpacing = 3; startY = 35; break;
      default: lineHeight = 25; charWidth = 11; charSpacing = 1; startY = 25; break;
    }
    // Total width per character including spacing
    int totalCharWidth = charWidth + charSpacing;
    
    // Word wrap the message with accent support
    int y = startY;
    int x = 10;
    int lineStartX = 10;
    int maxLineWidth = display.width() - 20;
    
    for (int i = 0; i < processedMessage.length(); i++) {
      char c = processedMessage[i];
      
      if (c == '\n' || c == '\r') {
        // New line
        y += lineHeight;
        x = lineStartX;
      } else {
        // Check if we need to wrap (use totalCharWidth which includes spacing)
        if (x + totalCharWidth > lineStartX + maxLineWidth && i > 0) {
          // Find last space to break at word boundary
          int lastSpace = -1;
          for (int j = i - 1; j >= 0 && j > i - 30; j--) {
            if (processedMessage[j] == ' ') {
              lastSpace = j;
              break;
            }
          }
          
          if (lastSpace >= 0) {
            // Rewind and print up to last space
            y += lineHeight;
            x = lineStartX;
            i = lastSpace + 1; // Skip the space
            continue;
          } else {
            // No space found, break here
            y += lineHeight;
            x = lineStartX;
          }
        }
        
        // Draw character with accent support
        int charWidthDrawn = drawCharWithAccent(c, x, y);
        // Add spacing between characters (already calculated in charSpacing)
        x += charWidthDrawn + charSpacing;
      }
    }
  }
  while (display.nextPage());
  
  Serial.println("Display updated!");
}

// HTML page with form
String getHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>E-Paper Message</title>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; background: #f0f0f0; padding: 20px; }";
  html += ".container { max-width: 400px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += "h1 { color: #333; }";
  html += "textarea { width: 100%; height: 150px; padding: 10px; font-size: 16px; border: 2px solid #ddd; border-radius: 5px; box-sizing: border-box; }";
  html += "button { background: #4CAF50; color: white; padding: 12px 30px; font-size: 16px; border: none; border-radius: 5px; cursor: pointer; margin-top: 10px; }";
  html += "button:hover { background: #45a049; }";
  html += ".current { margin-top: 20px; padding: 10px; background: #e8f5e9; border-radius: 5px; }";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>📱 E-Paper Message</h1>";
  html += "<form method='POST' action='/send' accept-charset='UTF-8'>";
  html += "<textarea name='message' placeholder='Enter your message here...' maxlength='200' style='font-family: Arial;'>" + displayMessage + "</textarea><br>";
  html += "<label for='fontsize' style='display: block; margin: 10px 0 5px 0; font-weight: bold;'>Font Size:</label>";
  html += "<select name='fontsize' id='fontsize' style='width: 100%; padding: 8px; font-size: 16px; border: 2px solid #ddd; border-radius: 5px; box-sizing: border-box;'>";
  html += "<option value='1'" + String(fontSize == 1 ? " selected" : "") + ">Small (9pt)</option>";
  html += "<option value='2'" + String(fontSize == 2 ? " selected" : "") + ">Medium (12pt)</option>";
  html += "<option value='3'" + String(fontSize == 3 ? " selected" : "") + ">Large (18pt)</option>";
  html += "<option value='4'" + String(fontSize == 4 ? " selected" : "") + ">X-Large (24pt)</option>";
  html += "</select><br>";
  html += "<button type='submit'>Send to Display</button>";
  html += "</form>";
  html += "<div class='current'><strong>Current:</strong><br>" + displayMessage + "</div>";
  html += "</div></body></html>";
  return html;
}

// Handle root page
void handleRoot() {
  Serial.println("Root page requested from: " + server.client().remoteIP().toString());
  server.send(200, "text/html; charset=UTF-8", getHTML());
  Serial.println("Root page sent successfully");
}

// Handle form submission
void handleSend() {
  if (server.hasArg("message")) {
    // Get the raw message - server.arg() should handle URL decoding
    displayMessage = server.arg("message");
    
    // Get font size if provided
    if (server.hasArg("fontsize")) {
      int newFontSize = server.arg("fontsize").toInt();
      if (newFontSize >= 1 && newFontSize <= 4) {
        fontSize = newFontSize;
        Serial.print("Font size changed to: ");
        Serial.println(fontSize);
      }
    }
    
    Serial.println("=== Raw message received ===");
    Serial.print("Length: ");
    Serial.println(displayMessage.length());
    Serial.print("Bytes: ");
    for (int i = 0; i < displayMessage.length(); i++) {
      Serial.print("0x");
      if ((unsigned char)displayMessage[i] < 16) Serial.print("0");
      Serial.print((unsigned char)displayMessage[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    Serial.print("As string: ");
    Serial.println(displayMessage);
    
    displayMessage.trim();
    
    if (displayMessage.length() == 0) {
      displayMessage = "Empty message";
    }
    
    // Save message and font size to NVS storage
    preferences.begin("epaper", false);
    preferences.putString("message", displayMessage);
    preferences.putInt("fontsize", fontSize);
    preferences.end();
    Serial.println("Message and font size saved to storage");
    
    // Update the display
    updateDisplay(displayMessage);
    showingQRCode = false; // Switch to message display
    
    // Send response
    server.send(200, "text/html; charset=UTF-8", getHTML() + "<script>setTimeout(function(){window.location.href='/';}, 2000);</script>");
    Serial.println("Message processed and displayed");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Initializing e-paper display...");
  
  // Initialize display
  display.init(115200, true, 2, false);
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  
  // Load saved message and font size from NVS storage
  preferences.begin("epaper", true); // true = read-only
  if (preferences.isKey("message")) {
    displayMessage = preferences.getString("message", "Hello World!");
    Serial.println("Loaded saved message: " + displayMessage);
  }
  if (preferences.isKey("fontsize")) {
    fontSize = preferences.getInt("fontsize", 2);
    Serial.print("Loaded saved font size: ");
    Serial.println(fontSize);
  }
  preferences.end();
  
  // Record boot time
  bootTime = millis();
  showingQRCode = true;
  
  // Start WiFi Access Point first
  Serial.println("Setting up WiFi Access Point...");
  WiFi.softAP(ssid, password);
  
  // Get the IP address that ESP32 assigned itself
  // ESP32 automatically assigns itself 192.168.4.1 when creating an AP
  // This is the default gateway IP for the access point network
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println("Note: ESP32 assigns itself this IP automatically");
  
  // Show QR codes on display (will switch to message after 1 minute)
  displayQRCode();
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.println("QR codes will display for 1 minute, then switch to saved message");
  
  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  
  // Catch-all handler - any other URL will show the message page
  server.onNotFound(handleRoot);
  
  // Start server
  server.begin();
  Serial.println("Web server started!");
  Serial.println("Connect to WiFi: " + String(ssid));
  Serial.println("Then open: http://" + IP.toString());
}

void loop() {
  // Handle web server requests
  server.handleClient();
  
  // Check if 1 minute has passed since boot and switch from QR codes to message
  if (showingQRCode && (millis() - bootTime >= QR_DISPLAY_DURATION)) {
    Serial.println("1 minute elapsed, switching to saved message...");
    showingQRCode = false;
    updateDisplay(displayMessage);
  }
  
  // Debug: Print connection status every 10 seconds
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 10000) {
    lastDebug = millis();
    unsigned long elapsed = millis() - bootTime;
    Serial.println("WiFi AP Status: " + String(WiFi.softAPgetStationNum()) + " connected");
    Serial.println("Server running on: http://" + WiFi.softAPIP().toString());
    if (showingQRCode) {
      Serial.print("Time until message display: ");
      Serial.print((QR_DISPLAY_DURATION - elapsed) / 1000);
      Serial.println(" seconds");
    }
  }
  
  delay(10);
}
