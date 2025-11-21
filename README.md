<div align="center">
  <h1>Super4C</h1> 
</div>
This is a cluster-type platform built for the Raspberry Pi CM5. This hardware is mini-ITX sized, supports four CM5 channels and expands nearly all of the CM5's interfaces. It also integrates an ESP32 module, enabling remote management capabilities for the entire cluster hardware. <br>
<br>

<div align="center">
  <img width="782" height="423" alt="super4c" src="https://github.com/user-attachments/assets/c9c0891d-67ae-43f1-b7c7-17af14ad42e8" />
</div>
<br>
<br>
<h2>What's in this repository?</h2>
This repository contains two software projects: one is developed using the Arduino IDE, providing basic web server control and status monitoring functions; the other is based on ESPHome, designed for integration with Home Assistant to enable more comprehensive lab integration and advanced automation control.

<h2>Arduino</h2>
Key features:  <br>
1. Integrated web server functionality  <br>
2. Wireless network access (initially via AP mode, switchable to STA mode after configuration)  <br>
3. Wi-Fi configuration for connection to your home or work network  <br>
4. On-board power supply current monitoring (6 channels)  <br>
5. On-board power switch control  <br>
6. Main information shown through OLED display  <br>
<br>
<div align="center">
  <img width="170" height="337" alt="image" src="https://github.com/user-attachments/assets/869e95f6-26a2-4f21-9a66-a6085ce4174f" />
</div>
<br>
<h2>ESPHome + Home Assistant</h2>
Ket features:  <br>
1. All the features supported in the previous Arduino project <br>
2. OTA functionality, no cable is needed  <br>
3. More comprehensive monitoring information (temperature/power/wifi stauts, etc.) and control modes  <br>
4. Can be directly integrated into Home Assistant  <br>
<br>
<div align="center">
  <img width="318" height="168" alt="a5f5ae53-f27e-4012-b82e-1b5e27d693a8" src="https://github.com/user-attachments/assets/1087f33a-e358-417c-89a8-caad27986f0d" />
</div>
<br>
<br>
More information: https://wiki.deskpi.com/super4c/
