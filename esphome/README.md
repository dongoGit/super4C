<h2>Usage Instructions:</h2>

1. Connect the cluster to a computer using a USB-to-UART cable, and install an OLED display (SSD1306 compatible). <br>

2. Before downloading the firmware, set the board to UART download mode by pressing both the EN and BOOT buttons simultaneously. Then release the EN button first, followed by the BOOT button.<br>

3. Build the project (./src/super4c.yaml) using the ESPHome builder and download the firmware.<br>

4. You will need the USB-to-UART cable for the first-time download, and then OTA will be useable.

5. Once the download is complete, press the EN button to restart the ESP32.<br>

7. On initial startup, the ESP32 will enter Access Point (AP) mode in one minute, you need to wait for that.<br>

8. Then, connect your computer to the Wi-Fi network with the SSID "Super4C". The default password is "12345678".<br>

9. Open a web browser and go to the IP address: http://192.168.4.1. This will bring up the Wi-Fi configuration page.<br>

10. Select your network's SSID and enter the password on that page, the device to switch into STA mode.<br>

11. Upon a successful connection in STA mode, the IP address assigned to the ESP32 will be shown on the OLED display. If no display is available, you can check the connected devices list in your router’s admin panel to find the IP address.<br>

12. Navigate to this IP address using your web browser to gain complete administrative control over the device's web server.<br>

13. Login into your Home Assistant server and enjoy the integration.

<h2>Other Tips</h2>
1. If you have forgotten the IP address, you can long-press the BOOT button for more than 5 seconds. The ESP32 will then return to AP mode, and you can please refer to the instructions above to proceed.<br>

2. There are two pages information shown on the OLED display, you can turn pages by a short press of the BOOT button.
<br>
<br>
Check this page for hardware instructions: https://wiki.deskpi.com/super4c/

