# Web Page Troubleshooting Guide

## Quick Checks

1. **Is your phone connected to the WiFi?**
   - Go to WiFi settings on your phone
   - Look for "ESP32-E-Paper" network
   - Make sure it shows "Connected" (not just "Saved")
   - Password is: `12345678`

2. **What IP address are you using?**
   - Check the e-paper display - it shows the IP address
   - Should be `192.168.4.1` (default)
   - Try: `http://192.168.4.1` or just `192.168.4.1`

3. **Check Serial Monitor Output**
   - Open serial monitor (115200 baud)
   - Look for:
     - "Setting up WiFi Access Point..."
     - "AP IP address: 192.168.4.1"
     - "Web server started!"
   - If you see errors, note them down

4. **Browser Issues**
   - Try a different browser (Chrome, Firefox, Safari)
   - Try incognito/private mode
   - Clear browser cache
   - Try typing just the IP: `192.168.4.1` (without http://)

5. **Network Issues**
   - Disconnect and reconnect to the WiFi
   - Forget the network and reconnect
   - Make sure you're not on a VPN
   - Try turning airplane mode on/off

## Common Problems

### "Can't reach this page" or "This site can't be reached"
- **Solution**: Make sure you're connected to "ESP32-E-Paper" WiFi, not your home WiFi

### Page loads but shows nothing
- **Solution**: Try a different browser or clear cache

### QR code doesn't work
- **Solution**: Manually connect using the WiFi name and password shown on display

### Serial monitor shows errors
- **Solution**: Note the error message and check if ESP32 needs to be reset

## Debug Steps

1. Check serial monitor for any error messages
2. Verify WiFi connection on phone
3. Try accessing `http://192.168.4.1` directly
4. Check if IP address matches what's on display
5. Try resetting the ESP32 (unplug and replug USB)

