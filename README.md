# NodeJS-NodeMCU WebSocket Project

A project demonstrating WebSocket communication between a NodeJS server and NodeMCU, with LED control functionality.

## Project Structure / Cấu trúc dự án

nodejs-nodemcu-ws/
├── api-server/
│ ├── .env
│ ├── package.json
│ ├── server.js
│ └── postman_collection.json
└── nodemcu-ws/
└── nodemcu-ws.ino

## Requirements / Yêu cầu

Hardware / Phần cứng:

- NodeMCU ESP8266
- LED
- Resistor 220Ω
- Jumper wires

Software / Phần mềm:

- Node.js (v12 or later)
- Arduino IDE
- Postman (for testing)

Arduino Libraries / Thư viện Arduino:

- ESP8266WiFi
- WebSocketsClient
- ArduinoJson

## Arduino IDE Setup / Cài đặt Arduino IDE

1. Add ESP8266 board support / Thêm hỗ trợ ESP8266:

   - Open File > Preferences
   - Add URL to "Additional Boards Manager URLs":
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
   - Go to Tools > Board > Boards Manager
   - Install "ESP8266 by ESP8266 Community"

2. Install required libraries / Cài đặt thư viện:

   - Go to Tools > Manage Libraries
   - Search and install:
     - "WebSockets" by Markus Sattler
     - "ArduinoJson" by Benoit Blanchon

3. Configure board / Cấu hình board:
   - Select Tools > Board > NodeMCU 1.0 (ESP-12E Module)
   - Set:
     - Upload Speed: 115200
     - CPU Frequency: 80MHz
     - Flash Size: 4MB (FS:2MB OTA:~1019KB)
     - Port: Select your NodeMCU COM port

## Setup & Installation / Cài đặt

API Server:

1. Install dependencies / Cài đặt thư viện:
   cd api-server
   npm install

2. Configure environment / Cấu hình môi trường:

   - Copy .env.example to .env
   - Update settings in .env:
     PORT=3000
     MQTT_BROKER=mqtt://your-broker-ip:1883

3. Start server / Khởi động server:
   npm start

NodeMCU:

1. Connect hardware / Kết nối phần cứng:

   - Connect LED to D1 pin (with 220Ω resistor)
   - Connect NodeMCU to computer via USB

2. Configure WiFi / Cấu hình WiFi:

   - Update ssid and password in nodemcu-ws.ino
   - Update ws_server with your API server IP address

3. Upload code / Nạp code:
   - Open nodemcu-ws.ino in Arduino IDE
   - Select correct board and port
   - Upload code to NodeMCU

## Testing / Kiểm thử

1. Import Postman Collection / Import collection Postman:

   - Import api-server/postman_collection.json to Postman

2. Available endpoints / Các endpoint có sẵn:
   - GET /api/status: Check server status / Kiểm tra trạng thái server
   - POST /api/control: Control LED / Điều khiển LED
     Turn ON: {"command": "LED_CONTROL", "value": "ON"}
     Turn OFF: {"command": "LED_CONTROL", "value": "OFF"}

## Circuit Diagram / Sơ đồ mạch

NodeMCU ESP8266 | LED
D1 (GPIO5) ---|>|--- 220Ω ---GND

## Features / Tính năng

- Real-time WebSocket communication / Giao tiếp WebSocket thời gian thực
- LED control through REST API / Điều khiển LED qua REST API
- Auto-reconnect on connection loss / Tự động kết nối lại khi mất kết nối
- Status monitoring / Giám sát trạng thái

## Troubleshooting / Xử lý sự cố

1. If NodeMCU can't connect / Nếu NodeMCU không kết nối được:

   - Check WiFi credentials / Kiểm tra thông tin WiFi
   - Verify server IP and port / Xác nhận IP và port của server
   - Check serial monitor for debug info / Xem thông tin debug trong serial monitor

2. If LED doesn't respond / Nếu LED không phản hồi:
   - Verify LED polarity / Kiểm tra cực của LED
   - Check pin connections / Kiểm tra kết nối chân
   - Verify WebSocket connection status / Kiểm tra trạng thái kết nối WebSocket

## License / Giấy phép

MIT License

## Contributing / Đóng góp

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Chào mừng các pull request. Với những thay đổi lớn, vui lòng mở issue trước để thảo luận về những gì bạn muốn thay đổi.
