/*
 * Project: Super4C (four channel cluster with Raspberry Pi CM5)
 * 
 * Description:
 * - Implements WiFi connectivity and web configuration interface
 * - Provides real-time status display on OLED screen
 * - Stores device configuration in non-volatile EEPROM
 * - Supports I2C peripheral communication
 * - Onboard ESP32 module: ESP32-WROOM-32E-N4
 * - Developed with Arduino IDE 2.3.6
 * - 
 * Dependencies:
 * - WiFi.h         (ESP32 built-in WiFi library)
 * - WebServer.h    (ESP32 built-in web server library)
 * - EEPROM.h       (Arduino EEPROM emulation library)
 * - U8g2lib.h      (Third-party OLED display library v2.35.30)
 * - Wire.h         (Arduino I2C communication library)
 * - SPI.h
 * - Ethernet.h
 * 
 * Hardware Requirements:
 * - Super4C board version v1.1 and above
 * - I2C OLED display (SSD1306 or compatible)
 * - Power supply input：19V DC
 * 
 * Created: 2025-08-01
 * Last Modified: 2025-09-04
 * 
 * Auther： dongo
 * SPDX-License-Identifier: MIT
 */


#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>

// define EEPROM address
#define EEPROM_SIZE 128
#define SSID_ADDR 0
#define PASS_ADDR 32

// IP reset button config
#define RESET_PIN 0            // use GPIO0 for reset
#define RESET_TIME_MS 5000     // hold reset for 5 second
unsigned long resetStartTime = 0;
bool resetTriggered = false;

// I2C pin config
#define CUSTOM_SDA 13
#define CUSTOM_SCL 15

// INA3221 config
#define SHUNT_RESISTOR 0.02f   // value of shunt resistor
#define INA3221_ADDR_1 0x40    // I2C adderss of first INA3221
#define INA3221_ADDR_2 0x41    // I2C adderss of second INA3221
#define INA3221_SHUNT 0x01     // registor of shunt resistor
#define INA3221_CONFIG 0x00    // register config

//W5500 pin config
#define SCK 18
#define MISO 19
#define MOSI 23 
#define SS 5

//ESP32 wired ethernet config(w5500)
IPAddress ip(192,168,1,222);
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
//EthernetServer serverENET(80);

// OLED config
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, CUSTOM_SCL, CUSTOM_SDA);

// global variable
WebServer server(80);
const long SCREEN_REFRESH_INTERVAL = 500;    // refreshing interval(ms)
unsigned long previousMillis = 0;            // time of previous refreshing

// GPIO definition
struct GPIOPin {
  uint8_t pin;
  const char* name;
  bool defaultState;
  const char* color;
  bool state;
};

// GPIO config
GPIOPin gpioConfig[] = {
  {2, "PSU #1", LOW, "#4CAF50", LOW},
  {4, "PSU #2", LOW, "#FF9800", LOW},
  {16, "PSU #3", LOW, "#2196F3", LOW},
  {17, "PSU #4", LOW, "#33FF33", LOW},
  {14, "CM5 #1", HIGH, "#FF33FF", HIGH},
  {27, "CM5 #2", HIGH, "#00BCD4", HIGH},
  {26, "CM5 #3", HIGH, "#FFEB3B", HIGH},
  {25, "CM5 #4", HIGH, "#F44336", HIGH}
};
const uint8_t GPIO_COUNT = sizeof(gpioConfig) / sizeof(gpioConfig[0]);

// current measurement
float dcPortCurrent[2] = {0, 0};     // current of DC input port
float psuCurrent[4] = {0, 0, 0, 0};  // current of each 5V PSU

// WiFi management
String savedSSID = "";
String savedPassword = "";
bool isAPMode = false;

// ====================== HTML for main page ======================
const char HTML_HEAD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Super4C ESP32 Controller</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
    body { background: linear-gradient(135deg, #1a2a6c, #b21f1f, #1a2a6c); color: white; min-height: 100vh; padding: 20px; }
    .container { max-width: 800px; margin: 0 auto; padding: 20px; }
    header { text-align: center; padding: 30px 0; margin-bottom: 30px; background: rgba(0,0,0,0.3); border-radius: 15px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); }
    h1 { font-size: 2.8rem; margin-bottom: 10px; text-shadow: 0 2px 4px rgba(0,0,0,0.5); }
    .subtitle { font-size: 1.2rem; opacity: 0.9; }
    .status-bar { display: flex; justify-content: center; background: rgba(0,0,0,0.4); padding: 15px; border-radius: 10px; margin: 20px 0; flex-wrap: wrap; }
    .status-item { margin: 5px 10px; }
    .gpio-grid { display: grid; grid-template-columns: repeat(2, minmax(280px, 1fr)); gap: 25px; margin-top: 30px; justify-content: center; }
    .gpio-card { background: rgba(255, 255, 255, 0.12); border-radius: 15px; padding: 25px; text-align: center; box-shadow: 0 8px 32px rgba(0,0,0,0.2); transition: all 0.3s ease; backdrop-filter: blur(10px); justify-content: center; }
    .gpio-card:hover { transform: translateY(-10px); box-shadow: 0 12px 40px rgba(0,0,0,0.3); }
    .gpio-name { font-size: 1.4rem; font-weight: 600; margin-bottom: 20px; color: #fff; }
    .state-indicator { display: inline-block; width: 24px; height: 24px; border-radius: 50%; margin-right: 10px; vertical-align: middle; }
    .state-text { font-size: 1.3rem; font-weight: 600; vertical-align: middle; }
    .toggle-btn { background: rgba(255,255,255,0.1); color: white; border: 2px solid rgba(255,255,255,0.3); padding: 14px 32px; font-size: 1.2rem; border-radius: 50px; margin-top: 25px; cursor: pointer; transition: all 0.3s ease; font-weight: 600; letter-spacing: 1px; width: 100%; }
    .toggle-btn:hover { background: rgba(255,255,255,0.2); transform: scale(1.05); }
    .all-controls { display: flex; gap: 15px; margin-top: 40px; justify-content: center; flex-wrap: wrap; width: 100%; }
    .control-btn { padding: 15px 30px; font-size: 1.1rem; border: none; border-radius: 10px; cursor: pointer; font-weight: 600; transition: all 0.3s ease; }
    .control-btn.switch-off { background: #F44336; color: white; }
    .control-btn.switch-on { background: #4CAF50; color: white; }
    .control-btn.toggle-all { background: #2196F3; color: white; }
    .control-btn.wifi-config { background: #9FB6CD; color: white; }
    .control-btn:hover { opacity: 0.9; transform: translateY(-3px); }
    footer { text-align: center; margin-top: 50px; padding: 20px; font-size: 0.9rem; opacity: 0.8; }
    @media (max-width: 768px) {
      .gpio-grid { grid-template-columns: 1fr; }
      h1 { font-size: 2.2rem; }
    }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <h1>Super4C</h1>
      <p class="subtitle">WiFi management and on board power control</p>
    </header>
    
    <div class="status-bar">
      <div class="status-item"><strong>Device IP:</strong> )rawliteral";

const char HTML_STATUS[] PROGMEM = R"rawliteral(</div>
      <div class="status-item"><strong>WiFi Signal:</strong> )rawliteral";

const char HTML_MODE[] PROGMEM = R"rawliteral(</div>
      <div class="status-item"><strong>Uptime:</strong> <span id="uptime">00:00:00</span></div>
      <div class="status-item"><strong>Mode:</strong> )rawliteral";

const char HTML_CURRENT_STATUS[] PROGMEM = R"rawliteral(
    </div>

    <div class="status-bar">
      <div class="status-item"><strong>DC1: </strong> <span id="DCPortCurrent0">0</span></div>
      <div class="status-item"><strong>DC2: </strong> <span id="DCPortCurrent1">0</span></div>
      <div class="status-item"><strong>PSU #1: </strong> <span id="PSUCurrent0">0</span></div>
      <div class="status-item"><strong>PSU #2: </strong> <span id="PSUCurrent1">0</span></div>
      <div class="status-item"><strong>PSU #3: </strong> <span id="PSUCurrent2">0</span></div>
      <div class="status-item"><strong>PSU #4: </strong> <span id="PSUCurrent3">0</span></div>
    </div>

    <div class="gpio-grid">
)rawliteral";

String generateHTML() {
  String html;
  html.reserve(8000); // memory pre-allocation
  
  // HTML head
  html += FPSTR(HTML_HEAD);
  html += WiFi.localIP().toString();
  html += FPSTR(HTML_STATUS);
  html += WiFi.RSSI();
  html += " dBm";
  html += FPSTR(HTML_MODE);
  html += (isAPMode ? "AP Mode" : "Station Mode");
  html += FPSTR(HTML_CURRENT_STATUS);

  // generate GPIO card
  for (int i = 0; i < GPIO_COUNT; i++) {
    bool isPSU = i < 4;
    bool state = gpioConfig[i].state;
    bool displayState = isPSU ? !state : state;
    const char* btnText = displayState ? "Switch OFF" : "Switch ON";
    
    html += "<div class=\"gpio-card\" style=\"border-top: 5px solid ";
    html += gpioConfig[i].color;
    html += ";\"><div class=\"gpio-name\">";
    html += gpioConfig[i].name;
    html += "</div><div class=\"state-display\"><span class=\"state-indicator\" style=\"background-color: ";
    html += displayState ? gpioConfig[i].color : "#555";
    html += ";\"></span><span class=\"state-text\">";
    html += displayState ? "ON" : "OFF";
    html += "</span></div><button class=\"toggle-btn\" onclick=\"toggleGPIO(";
    html += i;
    html += ")\">";
    html += btnText;
    html += "</button></div>";
  }

  // add control button 
  html += R"rawliteral(
    </div>
    
    <div class="all-controls">
      <button class="control-btn switch-off" onclick="setAll('off')">Switch All OFF</button>
      <button class="control-btn switch-on" onclick="setAll('on')">Switch All ON</button>
      <button class="control-btn toggle-all" onclick="toggleAll()">Toggle All</button>
    </div>
    
    <div class="all-controls">
      <button class="control-btn wifi-config" onclick="window.location.href='/wificonfig'">WiFi Setup</button>
    </div>

    <footer>
      <p>ESP32 Web Controller | Powered by 52Pi</p>
    </footer>
  </div>
  
  <script>
    let startTime = Date.now();
    const gpioColors = [)rawliteral";
  
  // add color for GPIO
  for (int i = 0; i < GPIO_COUNT; i++) {
    html += '"';
    html += gpioConfig[i].color;
    html += '"';
    if (i < GPIO_COUNT - 1) html += ',';
  }
  
  html += R"rawliteral(];
    
    function formatTime(seconds) {
      const hrs = Math.floor(seconds / 3600);
      const mins = Math.floor((seconds % 3600) / 60);
      const secs = seconds % 60;
      return `${hrs.toString().padStart(2, '0')}:${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
    }
    
    function updateUptime() {
      const elapsed = Math.floor((Date.now() - startTime) / 1000);
      document.getElementById('uptime').textContent = formatTime(elapsed);
    }
    
    function updateCurrent() {
      fetch('/current')
        .then(response => response.json())
        .then(data => {
          document.getElementById('DCPortCurrent0').textContent = data.dc1.toFixed(0) + "mA";
          document.getElementById('DCPortCurrent1').textContent = data.dc2.toFixed(0) + "mA";
          document.getElementById('PSUCurrent0').textContent = data.psu1.toFixed(0) + "mA";
          document.getElementById('PSUCurrent1').textContent = data.psu2.toFixed(0) + "mA";
          document.getElementById('PSUCurrent2').textContent = data.psu3.toFixed(0) + "mA";
          document.getElementById('PSUCurrent3').textContent = data.psu4.toFixed(0) + "mA";
        })
        .catch(error => console.error('Error fetching current data:', error));
    }
    
    function updateGPIOState(index, state) {
      const elements = document.querySelectorAll('.state-indicator, .state-text, .toggle-btn');
      const isPSU = index < 4;
      const displayState = isPSU ? !state : state;
      
      elements[index*3].style.backgroundColor = displayState ? gpioColors[index] : '#555';
      elements[index*3+1].textContent = displayState ? 'ON' : 'OFF';
      elements[index*3+2].textContent = displayState ? 'Switch OFF' : 'Switch ON';
    }
    
    function toggleGPIO(index) {
      fetch('/toggle?pin=' + index).then(r => r.json()).then(data => {
        if(data.success) updateGPIOState(index, data.state);
      });
    }
    
    function setAll(state) {
      fetch('/setall?state=' + state).then(r => r.json()).then(data => {
        if(data.success) {
          for(let i = 0; i < )rawliteral";
  html += GPIO_COUNT;
  html += R"rawliteral(; i++) {
            updateGPIOState(i, i < 4 ? (state === 'off') : (state === 'on'));
          }
        }
      });
    }
    
    function toggleAll() {
      fetch('/toggleall').then(r => r.json()).then(data => {
        if(data.success) {
          for(let i = 0; i < )rawliteral";
  html += GPIO_COUNT;
  html += R"rawliteral(; i++) updateGPIOState(i, data.states[i]);
        }
      });
    }
    
    // init
    updateUptime();
    updateCurrent();
    setInterval(updateUptime, 1000);
    setInterval(updateCurrent, 1000);
  </script>
</body>
</html>)rawliteral";
  
  return html;
}

// ====================== HTML for WiFi configuration page ======================
String generateWiFiConfigHTML() {
  String html;
  html.reserve(3000);
  
  html += R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>WiFi Configuration</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
    body { background: linear-gradient(135deg, #1a2a6c, #b21f1f, #1a2a6c); color: white; min-height: 100vh; padding: 20px; display: flex; justify-content: center; align-items: center; }
    .container { max-width: 600px; width: 100%; background: rgba(255, 255, 255, 0.12); border-radius: 20px; padding: 40px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); backdrop-filter: blur(10px); }
    header { text-align: center; margin-bottom: 30px; }
    h1 { font-size: 2.5rem; margin-bottom: 10px; text-shadow: 0 2px 4px rgba(0,0,0,0.5); }
    .subtitle { font-size: 1.1rem; opacity: 0.8; }
    .form-group { margin-bottom: 25px; }
    label { display: block; margin-bottom: 8px; font-weight: 600; font-size: 1.1rem; }
    input { width: 100%; padding: 15px; border-radius: 12px; border: 2px solid rgba(255,255,255,0.3); background: rgba(255,255,255,0.1); color: white; font-size: 1.1rem; transition: all 0.3s ease; }
    input:focus { outline: none; border-color: #2196F3; background: rgba(255,255,255,0.2); }
    .btn-group { display: flex; gap: 15px; margin-top: 30px; }
    .btn { flex: 1; padding: 16px; border-radius: 12px; border: none; font-size: 1.1rem; font-weight: 600; cursor: pointer; transition: all 0.3s ease; }
    .btn-save { background: #4CAF50; color: white; }
    .btn-cancel { background: #F44336; color: white; }
    .btn:hover { opacity: 0.9; transform: translateY(-3px); }
    .status-bar { margin-top: 25px; padding: 15px; background: rgba(0,0,0,0.3); border-radius: 12px; text-align: center; }
    .status-item { margin: 8px 0; }
    footer { text-align: center; margin-top: 30px; padding-top: 20px; border-top: 1px solid rgba(255,255,255,0.2); font-size: 0.9rem; opacity: 0.8; }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <h1>WiFi Configuration</h1>
      <p class="subtitle">Configure your WiFi settings</p>
    </header>
    
    <form id="wifiForm" action="/savewifi" method="POST">
      <div class="form-group">
        <label for="ssid">WiFi SSID</label>
        <input type="text" id="ssid" name="ssid" placeholder="Enter your WiFi name" required>
      </div>
      
      <div class="form-group">
        <label for="password">WiFi Password</label>
        <input type="password" id="password" name="password" placeholder="Enter your WiFi password" required>
      </div>
      
      <div class="btn-group">
        <button type="button" class="btn btn-cancel" onclick="window.location.href='/'">Cancel</button>
        <button type="submit" class="btn btn-save">Save & Reboot</button>
      </div>
    </form>
    
    <div class="status-bar">
      <div class="status-item">
        <strong>Current Mode:</strong> )rawliteral";
  
  html += (isAPMode ? "AP Mode" : "Station Mode");
  html += R"rawliteral(
      </div>
      <div class="status-item">
        <strong>Device IP:</strong> )rawliteral";
  
  html += WiFi.localIP().toString();
  html += R"rawliteral(
      </div>
    </div>
    
    <footer>
      <p>ESP32 WiFi Configuration | Device will reboot after saving</p>
    </footer>
  </div>
  
  <script>
    document.addEventListener('DOMContentLoaded', function() {
      const savedSSID = localStorage.getItem('wifi_ssid') || '';
      const savedPassword = localStorage.getItem('wifi_password') || '';
      
      document.getElementById('ssid').value = savedSSID;
      document.getElementById('password').value = savedPassword;
      
      document.getElementById('wifiForm').addEventListener('submit', function() {
        localStorage.setItem('wifi_ssid', document.getElementById('ssid').value);
        localStorage.setItem('wifi_password', document.getElementById('password').value);
      });
    });
  </script>
</body>
</html>)rawliteral";
  
  return html;
}

// ====================== EEPROM management ======================
void readWiFiCredentials() {
  EEPROM.begin(EEPROM_SIZE);
  
  char buffer[64];
  // read SSID
  for (int i = 0; i < 32; i++) {
    buffer[i] = EEPROM.read(SSID_ADDR + i);
    if (buffer[i] == 0) break;
  }
  savedSSID = buffer;
  
  // read password
  for (int i = 0; i < 64; i++) {
    buffer[i] = EEPROM.read(PASS_ADDR + i);
    if (buffer[i] == 0) break;
  }
  savedPassword = buffer;
  
  EEPROM.end();
}

void saveWiFiCredentials(const String& ssid, const String& password) {
  EEPROM.begin(EEPROM_SIZE);
  
  // save SSID
  for (int i = 0; i < 32; i++) {
    EEPROM.write(SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
  }
  
  // save password
  for (int i = 0; i < 64; i++) {
    EEPROM.write(PASS_ADDR + i, i < password.length() ? password[i] : 0);
  }
  
  EEPROM.commit();
  EEPROM.end();
  
  savedSSID = ssid;
  savedPassword = password;
}

void clearWiFiCredentials() {
  EEPROM.begin(EEPROM_SIZE);
  
  // clear the credentials
  for (int i = 0; i < 96; i++) {
    EEPROM.write(SSID_ADDR + i, 0);
  }
  
  EEPROM.commit();
  EEPROM.end();
  
  savedSSID = "";
  savedPassword = "";
}

// ====================== server routing ======================
void handleRoot() {
  server.send(200, "text/html", generateHTML());
}

void handleWiFiConfig() {
  server.send(200, "text/html", generateWiFiConfigHTML());
}

void handleSaveWiFi() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    if (ssid.length() > 0) {
      saveWiFiCredentials(ssid, password);
      
      String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>WiFi Saved</title>
  <style>
    body { background: linear-gradient(135deg, #1a2a6c, #b21f1f); color: white; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; text-align: center; padding: 20px; }
    .container { background: rgba(255,255,255,0.1); border-radius: 20px; padding: 40px; max-width: 600px; width: 100%; backdrop-filter: blur(10px); box-shadow: 0 8px 32px rgba(0,0,0,0.3); }
    h1 { font-size: 2.5rem; margin-bottom: 20px; color: #4CAF50; }
    p { font-size: 1.2rem; margin-bottom: 30px; line-height: 1.6; }
    .countdown { font-size: 1.5rem; font-weight: bold; margin: 30px 0; color: #FFEB3B; }
    .info { background: rgba(0,0,0,0.3); padding: 20px; border-radius: 15px; margin: 25px 0; text-align: left; }
  </style>
</head>
<body>
  <div class="container">
    <h1>WiFi Settings Saved!</h1>
    <p>Your WiFi credentials have been saved.</p>
    
    <div class="info">
      <p><strong>SSID:</strong> )rawliteral";
      html += ssid;
      html += R"rawliteral(</p>
      <p><strong>Password:</strong> ********</p>
    </div>
    
    <p>Device rebooting in <span id="countdown">10</span> seconds...</p>
    <div class="countdown">Please reconnect to your WiFi</div>
  </div>
  
  <script>
    let seconds = 10;
    const countdownEl = document.getElementById('countdown');
    const timer = setInterval(() => {
      seconds--;
      countdownEl.textContent = seconds;
      if (seconds <= 0) clearInterval(timer);
    }, 1000);
    setTimeout(() => window.location.href = '/', 10000);
  </script>
</body>
</html>)rawliteral";
      
      server.send(200, "text/html", html);
      delay(10000);
      ESP.restart();
      return;
    }
  }
  
  server.send(400, "text/plain", "Invalid request");
}

void handleToggle() {
  if (server.hasArg("pin")) {
    int pinIndex = server.arg("pin").toInt();
    
    if (pinIndex >= 0 && pinIndex < GPIO_COUNT) {
      // update GPIO status
      gpioConfig[pinIndex].state = !gpioConfig[pinIndex].state;
      digitalWrite(gpioConfig[pinIndex].pin, gpioConfig[pinIndex].state);
      
      // Send JSON response
      server.send(200, "application/json", 
        "{\"success\":true,\"state\":" + String(gpioConfig[pinIndex].state) + "}");
      return;
    }
  }
  
  server.send(400, "text/plain", "Invalid request");
}

void handleSetAll() {
  if (server.hasArg("state")) {
    String stateArg = server.arg("state");
    bool newState = (stateArg == "off");
    
    for (int i = 0; i < GPIO_COUNT; i++) {
      bool state = (i < 4) ? newState : !newState;
      gpioConfig[i].state = state;
      digitalWrite(gpioConfig[i].pin, state);
    }

    server.send(200, "application/json", 
      "{\"success\":true,\"state\":" + String(newState) + "}");
    return;
  }
  
  server.send(400, "text/plain", "Invalid request");
}

void handleToggleAll() {
  String json = "{\"success\":true,\"states\":[";
  
  for (int i = 0; i < GPIO_COUNT; i++) {
    gpioConfig[i].state = !gpioConfig[i].state;
    digitalWrite(gpioConfig[i].pin, gpioConfig[i].state);
    
    json += gpioConfig[i].state;
    if (i < GPIO_COUNT - 1) json += ',';
  }
  
  json += "]}";
  server.send(200, "application/json", json);
}

void handleCurrentData() {
  String json = "{";
  json += "\"dc1\":" + String(dcPortCurrent[0]) + ",";
  json += "\"dc2\":" + String(dcPortCurrent[1]) + ",";
  json += "\"psu1\":" + String(psuCurrent[0]) + ",";
  json += "\"psu2\":" + String(psuCurrent[1]) + ",";
  json += "\"psu3\":" + String(psuCurrent[2]) + ",";
  json += "\"psu4\":" + String(psuCurrent[3]);
  json += "}";
  
  server.send(200, "application/json", json);
}

// ====================== INA3221 current measurement ======================
void writeINA3221Register(uint8_t address, uint8_t reg, uint16_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write((value >> 8) & 0xFF);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}

int16_t readShuntVoltage(uint8_t channel) {
  uint8_t address = (channel < 4) ? INA3221_ADDR_1 : INA3221_ADDR_2;
  uint8_t reg = INA3221_SHUNT + ((channel-1) % 3)*2;
  
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(address, 2);
  return (Wire.read() << 8) | Wire.read();
}

float readCurrent(uint8_t channel) {
  int16_t shunt_raw = readShuntVoltage(channel);
  float shunt_V = (shunt_raw >> 2) * 0.00004f; // LSB=40μV 
  return (shunt_V / SHUNT_RESISTOR) * 1000.0f; // convert to mA
}

void updateCurrentMeasurements() {
  for (uint8_t ch = 1; ch <= 6; ch++) {
    float current = readCurrent(ch);
    
    if (ch < 3) {
      dcPortCurrent[ch-1] = current;
    } else if (ch < 7) {
      psuCurrent[ch-3] = current;
    }
  }
}

// ====================== OLED display ======================
void drawStaticContent() {
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 40, "DC1:     A  #2:     A");
  u8g2.drawStr(0, 50, "DC2:     A  #3:     A");
  u8g2.drawStr(0, 60, " #1:     A  #4:     A");
}

void updateOLED() {
//  u8g2.clearBuffer();
  
  // wifi IP address display
  u8g2.setFont(u8g2_font_6x10_tr);
  if (WiFi.status() == WL_CONNECTED) {
    String ip = WiFi.localIP().toString();
    u8g2.drawStr(85, 10, "Super4C");
    u8g2.drawStr(0, 15, "Station mode");
    u8g2.drawStr(0, 25, ip.c_str());
  } else {
    String ip = "AP mode: " + WiFi.softAPIP().toString();
    u8g2.drawStr(0, 10, ip.c_str());
    u8g2.drawStr(0, 20, "SSID: Super4C");
    u8g2.drawStr(0, 30, "PWD: 12345678");
  }
  
  //w5500 link status display
  if (Ethernet.linkStatus() == LinkON) {
    u8g2.drawStr(95, 25, "| W55");
  } else {
    u8g2.drawStr(95, 25, "| OFF");
  }

  //current display
  for (int i = 0; i < 2; i++) {
    u8g2.setCursor(28, 40 + i*10);
    u8g2.print(dcPortCurrent[i]/1000, 2);
  }
  
  for (int i = 0; i < 4; i++) {
    int x = (i < 1) ? 28 : 93;
    int y = (i < 1) ? 60 : 40 + (i-1)*10;
    u8g2.setCursor(x, y);
    u8g2.print(psuCurrent[i]/1000, 2);
  }
  
  u8g2.sendBuffer();
}

// ====================== reset button management ======================
void checkResetButton() {

  if (digitalRead(RESET_PIN)) { // reset button release
    resetStartTime = 0;
    resetTriggered = false;
    return;
  }
  
  // button pressed
  if (resetStartTime == 0) {
    resetStartTime = millis();
    Serial.println("Reset button pressed");
    return;
  }
  
  // reset timeout detection
  if (!resetTriggered && (millis() - resetStartTime) >= RESET_TIME_MS) {
    resetTriggered = true;
    Serial.println("Entering AP mode");
    clearWiFiCredentials();
    Serial.println("Restarting device...");
    delay(1000);
    ESP.restart();
  }
}

// ====================== main ======================
void setup() {
  Serial.begin(115200);
  
  // HW init
  pinMode(RESET_PIN, INPUT_PULLUP);
  Wire.begin(CUSTOM_SDA, CUSTOM_SCL);
  
  // INA3221 init
  writeINA3221Register(INA3221_ADDR_1, INA3221_CONFIG, 0x7127); // 复位+连续测量
  writeINA3221Register(INA3221_ADDR_2, INA3221_CONFIG, 0x7127);
  delay(50);
  
  // OLED init
  u8g2.begin();
  u8g2.setAutoPageClear(0);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.firstPage();
  do { drawStaticContent(); } while (u8g2.nextPage());
  
  // GPIO init
  for (int i = 0; i < GPIO_COUNT; i++) {
    pinMode(gpioConfig[i].pin, OUTPUT);
    digitalWrite(gpioConfig[i].pin, gpioConfig[i].defaultState);
    gpioConfig[i].state = gpioConfig[i].defaultState;
  }
  
  // WiFi Cconnection
  readWiFiCredentials();
  if (savedSSID.length() > 0) {
    Serial.printf("Connecting to WiFi: %s\n", savedSSID.c_str());
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
    
    for (int i = 0; i < 20; i++) {
      if (WiFi.status() == WL_CONNECTED) break;
      delay(500);
      Serial.print(".");
    }
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    isAPMode = true;
    Serial.println("\nStarting AP mode");
    WiFi.softAP("Super4C", "12345678");
    Serial.println("AP IP: " + WiFi.softAPIP().toString());
  } else {
    isAPMode = false;
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
  }
  
  // Webserver routing config
  server.on("/", handleRoot);
  server.on("/wificonfig", handleWiFiConfig);
  server.on("/savewifi", HTTP_POST, handleSaveWiFi);
  server.on("/toggle", handleToggle);
  server.on("/setall", handleSetAll);
  server.on("/toggleall", handleToggleAll);
  server.on("/current", handleCurrentData); 
  
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Hold GPIO0 for 5s to reset");

  //W5500 init and start
  Ethernet.init(SS);
  delay(100);
  Ethernet.begin(mac, ip);

  auto hwStatus = Ethernet.hardwareStatus();
  switch(hwStatus){
    case !EthernetW5500:
      Serial.println("Ethernet HW not detected");
      break;
    case EthernetW5500:
      Serial.print("Ethernet W5500 installed, ");
      break;
  }

  auto link = Ethernet.linkStatus();
  switch(link){
    case Unknown:
      Serial.println("Unknown");
      break;
    case LinkON:
      Serial.println("Link ON");
      break;
    case LinkOFF:
      Serial.println("Link OFF");
      break;      
  } 
  
}

void loop() {
  unsigned long currentMillis = millis();
  
  // scheduled task
  if (currentMillis - previousMillis >= SCREEN_REFRESH_INTERVAL) {
    updateCurrentMeasurements();
    updateOLED();
    previousMillis = currentMillis;
  }
  
  // Handle button and network requests
  checkResetButton();
  server.handleClient();
}