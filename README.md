<div align="center">
  <h1>Super4C</h1> 
</div>
This is a cluster-type platform built for the Raspberry Pi CM5. This hardware is mini-ITX sized, supports four CM5 channels and expands nearly all of the CM5's interfaces. It also integrates an ESP32 module, enabling remote management capabilities for the entire cluster hardware.


<img width="3128" height="1692" alt="super4c" src="https://github.com/user-attachments/assets/c9c0891d-67ae-43f1-b7c7-17af14ad42e8" />


More information of this board : https://wiki.deskpi.com/super4c/

<h2>What's in this repository?</h2>
This repository contains two software projects: one is developed using the Arduino IDE, providing basic web server control and status monitoring functions; the other is based on ESPHome, designed for integration with Home Assistant to enable more comprehensive lab integration and advanced automation control.

<h2>Arduino</h2>
Key features:  <br>
1. Integrated web server functionality  <br>
2. Wireless network access (initially via AP mode, switchable to STA mode after configuration)  <br>
3. Wi-Fi configuration for connection to your home or work network  <br>
4. On-board power supply current monitoring (6 channels)  <br>
5. On-board power switch control  <br>
6. Main information shown through OLED display

<div align="center">
  <img width="170" height="337" alt="image" src="https://github.com/user-attachments/assets/869e95f6-26a2-4f21-9a66-a6085ce4174f" />
  <img width="225" height="126" alt="image" src="https://github.com/user-attachments/assets/d1ca3134-5e86-4560-99a1-0a17ecef8694" />
</div>


<h2>ESPHome + Home Assistant</h2>
Ket features:  <br>
1. All the features supported in the previous Arduino project <br>
2. OTA functionality, no cable is needed  <br>
3. More comprehensive monitoring information (temperature/power/wifi stauts, etc.) and control modes  <br>
4. Can be directly integrated into Home Assistant

<img width="333" height="257" alt="image" src="https://github.com/user-attachments/assets/5deb1a12-16ad-4b67-a1bd-905d93a5ba70" />
<img width="256" height="536" alt="be62a63b-e9d2-46ad-a3e5-574df0bb515c" src="https://github.com/user-attachments/assets/4fe90b6e-de49-4f53-8e1c-ae0f3e2195e1" />



