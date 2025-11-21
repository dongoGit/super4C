<div align="center">
  <h1>Super4C</h1> 
</div>
This is a cluster-type platform built for the Raspberry Pi CM5. This hardware is mini-ITX sized, supports four CM5 channels and expands nearly all of the CM5's interfaces. It also integrates an ESP32 module, enabling remote management capabilities for the entire cluster hardware.


![super4c](./images/super4c.png)

More information of this board : https://wiki.deskpi.com/super4c/

<h2>What's in this repository?</h2>
This repository contains two software projects: one is developed using the Arduino IDE, providing basic web server control and status monitoring functions; the other is based on ESPHome, designed for integration with Home Assistant to enable more comprehensive lab integration and advanced automation control.

<h3>Arduino</h3>
Key features:  <br>
1. Integrated web server functionality  <br>
2. Wireless network access (initially via AP mode, switchable to STA mode after configuration)  <br>
3. Wi-Fi configuration for connection to your home or work network  <br>
4. On-board power supply current monitoring (6 channels)  <br>
5. On-board power switch control  <br>
6. Main information shown through OLED display

<div align="center">
  <img width="341" height="675" alt="image" src="https://github.com/user-attachments/assets/869e95f6-26a2-4f21-9a66-a6085ce4174f" />
  <img width="450" height="253" alt="image" src="https://github.com/user-attachments/assets/d1ca3134-5e86-4560-99a1-0a17ecef8694" />
  <img width="451" height="250" alt="image" src="https://github.com/user-attachments/assets/211b158e-c6d0-4ec8-a3ea-41da1029ff71" />
</div>

<h3>ESPHOME + Home Assistant</h3>
Ket features:  <br>
1. 
