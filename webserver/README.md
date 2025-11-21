<h2>Usage Instructions:</h2>

1. Connect the cluster to a computer using a USB-to-UART cable, and install an OLED display (SSD1306 compatible). <br>

2. Before downloading the firmware, set the board to UART download mode by pressing both the EN and BOOT buttons simultaneously. Then release the EN button first, followed by the BOOT button.<br>

3. Build the project (./src/esp32_webserver_1.1.2.ino) using the Arduino IDE and download the firmware.<br>

4. Once the download is complete, press the EN button to restart the ESP32.<br>

5. On initial startup, the ESP32 will enter Access Point (AP) mode.<br>

6. Connect your computer to the Wi-Fi network with the SSID "Super4C". The default password is "12345678".<br>

7. Open a web browser and go to the IP address: http://192.168.4.1. This will bring up the Wi-Fi configuration page.<br>

8. Enter the SSID and password of your Wi-Fi network, then save the settings and reboot the device and wait for about 10s.<br>

9. After reboot, the new IP address assigned to the ESP32 will be shown on the OLED display. If no display is available, you can check the connected devices list in your router’s admin panel to find the IP address.<br>

10. Enter this new IP address into your web browser to access and fully control the web server.<br><br>

<h2>Other Tips</h2>
If you have forgotten the IP address, you can long-press the BOOT button for more than 5 seconds. The ESP32 will then return to AP mode, and you can please refer to the instructions above to proceed.<br>
<br>
Check this page for hardware instructions: https://wiki.deskpi.com/super4c/
