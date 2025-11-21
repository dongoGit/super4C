<div align="center">
  <h1>Super4C</h1> 
</div>
This is a cluster-type platform built for the Raspberry Pi CM5. This hardware is mini-ITX sized, supports four CM5 channels and expands nearly all of the CM5's interfaces. It also integrates an ESP32 module, enabling remote management capabilities for the entire cluster hardware.


![super4c](./images/super4c.png)

More information of this board : https://wiki.deskpi.com/super4c/

<h2>What's in this repository?</h2>
This repository contains two software projects: one is developed using the Arduino IDE, providing basic web server control and status monitoring functions; the other is based on ESPHome, designed for integration with Home Assistant to enable more comprehensive lab integration and advanced automation control.

<h3>Arduino</h3>
Main features:  <br>
1. Integrated web server functionality  <br>
2. Wireless network access (initially via AP mode, switchable to STA mode after configuration)  <br>
3. Wi-Fi configuration for connection to your home or work network  <br>
4. On-board power supply current monitoring (6 channels)  <br>
5. On-board power switch control  <br>

<img width="341" height="675" alt="image" src="https://github.com/user-attachments/assets/869e95f6-26a2-4f21-9a66-a6085ce4174f" />
