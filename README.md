<div align="center">
  <h1>Super4C</h1> 
</div>
This is a cluster-type platform built for the Raspberry Pi CM5. This hardware is mini-ITX sized, supports four CM5 channels and expands nearly all of the CM5's interfaces. It also integrates an ESP32 module, enabling remote management capabilities for the entire cluster hardware. <br>
<br>

<div align="center">
  <img width="782" height="423" alt="super4c" src="https://github.com/user-attachments/assets/c9c0891d-67ae-43f1-b7c7-17af14ad42e8" />
</div>
<h2>What's in this repository?</h2>
This repository contains two software projects: one is a simple webserver developed with the Arduino IDE, providing basic control and status monitoring functions; the other is based on ESPHome, and can be integration into your Home Assistant to enable more comprehensive lab integration and advanced automation control.

<h2>Simple webserver</h2>
Key features:  <br>
1. Integrated web server for remote control and monitoring  <br>
2. Wireless network access (initially via AP mode, switchable to STA mode after configuration)  <br>
3. Wi-Fi configuration for connection to your home or work network  <br>
4. Power supply current monitoring (6 channels)  <br>
5. Power switch control  <br>
6. System status display via OLED screen  <br>
<br>
<div align="center">
  <img width="170" height="337" alt="image" src="https://github.com/user-attachments/assets/869e95f6-26a2-4f21-9a66-a6085ce4174f" />
</div>
<h2>ESPHome + Home Assistant</h2>
Ket features:  <br>
1. All the features supported in the previous webserver project <br>
2. OTA (Over-The-Air) update capability, enabling wireless firmware upgrades  <br>
3. Enhanced data monitoring (e.g., temperature, power, Wi-Fi status) and flexible control modes  <br>
4. Seamless integration with Home Assistant for native smart home automation  <br>
<br>
<div align="center">
  <img width="424" height="224" alt="a5f5ae53-f27e-4012-b82e-1b5e27d693a8" src="https://github.com/user-attachments/assets/1087f33a-e358-417c-89a8-caad27986f0d" />
</div>
More information: https://wiki.deskpi.com/super4c/
