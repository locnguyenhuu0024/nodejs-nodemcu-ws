# Quick Start Guide: NodeJS-NodeMCU-Arduino WebSocket Project

This guide will help you quickly set up and run the NodeJS-NodeMCU-Arduino WebSocket project. For more detailed information, refer to the README.md file.

## Prerequisites

- Arduino Mega 2560
- ESP8266 WiFi module
- NodeMCU ESP8266
- Servo motor
- LED
- Jumper wires
- USB cables
- Computer with Arduino IDE and Node.js installed

## Step 1: Set Up the Server

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/nodejs-nodemcu-ws.git
   cd nodejs-nodemcu-ws
   ```

2. Install server dependencies:
   ```bash
   cd api-server
   npm install
   ```

3. Start the server:
   ```bash
   npm start
   ```

4. Note your server's IP address (you'll need this later):
   ```bash
   # On Linux/Mac
   ifconfig
   
   # On Windows
   ipconfig
   ```

## Step 2: Set Up the NodeMCU

1. Open Arduino IDE
2. Install required libraries:
   - Go to Sketch > Include Library > Manage Libraries
   - Install "ArduinoJson" (version 6.x)
   - Install "WebSockets" by Markus Sattler

3. Open the `nodemcu-ws.ino` file from the `nodemcu-ws` directory

4. Update WiFi credentials and server information:
   ```cpp
   const char* ssid = "your_wifi_ssid";
   const char* password = "your_wifi_password";
   const char* ws_server = "your_server_ip";
   const int ws_port = 8080;
   ```

5. Upload the code to your NodeMCU
6. Open Serial Monitor (115200 baud) to verify connection

## Step 3: Set Up the Arduino with ESP8266

1. Create a `config.h` file in the `arduino-esp8266` directory with the following content:
   ```cpp
   #ifndef CONFIG_H
   #define CONFIG_H
   
   // WiFi credentials
   #define WIFI_SSID "your_wifi_ssid"
   #define WIFI_PASSWORD "your_wifi_password"
   
   // WebSocket server settings
   #define WS_SERVER "your_server_ip"
   #define WS_PORT "8080"
   
   // Serial communication settings
   #define SERIAL_BAUD 115200
   
   #endif
   ```

2. Connect the ESP8266 to Arduino Mega:
   - ESP8266 TX → Arduino RX1 (Pin 19)
   - ESP8266 RX → Arduino TX1 (Pin 18)
   - ESP8266 CH_PD → Arduino 3.3V
   - ESP8266 VCC → Arduino 3.3V
   - ESP8266 GND → Arduino GND

3. Connect the servo to Arduino Mega:
   - Servo signal wire → Arduino Pin 9
   - Servo power wire → Arduino 5V
   - Servo ground wire → Arduino GND

4. Open the `arduino-esp8266.ino` file from the `arduino-esp8266` directory

5. Upload the code to your Arduino Mega

6. Open Serial Monitor (115200 baud) to verify connection

## Step 4: Test the System

### Using Postman

1. Import the Postman collection:
   - Open Postman
   - Click "Import" and select the `api-server/postman_collection.json` file
   - Set the `base_url` environment variable to `http://your_server_ip:8080`

2. Test device status:
   - Send the "Get All Devices Status" request
   - You should see status information for both devices

3. Test LED control:
   - Send the "Turn LED ON" request with target "nodemcu"
   - The LED connected to NodeMCU should turn on

4. Test servo control:
   - Send the "Set Servo to 90 degrees" request with target "arduino"
   - The servo connected to Arduino should move to 90 degrees

### Using API Endpoints Directly

1. Check device status:
   ```
   GET http://your_server_ip:8080/status
   ```

2. Control LED:
   ```
   POST http://your_server_ip:8080/led
   Content-Type: application/json
   
   {
     "state": "on",
     "target": "nodemcu"
   }
   ```

3. Control servo:
   ```
   POST http://your_server_ip:8080/servo
   Content-Type: application/json
   
   {
     "angle": 90,
     "target": "arduino"
   }
   ```

## Troubleshooting

If you encounter issues:

1. Check Serial Monitor output for both devices
2. Verify WiFi credentials and server IP address
3. Ensure server is running and accessible
4. For Arduino ESP8266, try these debug commands in Serial Monitor:
   - `status` - Check connection status
   - `reset` - Reset ESP8266
   - `connect` - Force connection attempt
   - `ip` - Try direct IP connection

For more detailed troubleshooting, refer to the `troubleshooting.md` file.

## Next Steps

- Explore the API endpoints in the Postman collection
- Modify the code to add new features
- Add additional sensors to the NodeMCU or Arduino
- Create a web interface to control the devices

Enjoy your IoT project! 