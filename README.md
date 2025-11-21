<div align="center">
  <h1>Super4C</h1> 
</div>
This is a cluster-type platform built for the Raspberry Pi CM5. This hardware is mini-ITX sized, supports four CM5 channels and expands nearly all of the CM5's interfaces. It also integrates an ESP32 module, enabling remote management capabilities for the entire cluster hardware.


![super4c](./images/super4c.png)

More information of this board : https://wiki.deskpi.com/super4c/

<h2>What's in this repo?</h2>
This repository contains two software projects: one is developed using the Arduino IDE, providing basic web server control and status monitoring functions; the other is based on ESPHome, designed for integration with Home Assistant to enable more comprehensive lab integration and advanced automation control.

<h3>Arduino</h3>
Main features:  
1. Integrated web server functionality  
2. Wireless network access (initially via AP mode, switchable to STA mode after configuration)  
3. Wi-Fi configuration for connection to your home or work network  
4. On-board power supply current monitoring (6 channels)  
5. On-board power switch control  
