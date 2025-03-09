# NodeMCU ESP8266 MQTT Sensor API

Hệ thống thu thập dữ liệu từ các cảm biến kết nối với NodeMCU ESP8266 thông qua MQTT và cung cấp API để truy cập dữ liệu.

## Cấu trúc dự án

```
nodejs-nodemcu-ws/
├── arduino-esp8266/
│   └── arduino-esp8266.ino    # Code Arduino cho NodeMCU ESP8266
├── api-server/
│   ├── server.js              # API server Node.js
│   ├── package.json           # Cấu hình npm
│   └── .env                   # Cấu hình môi trường
└── mqtt-broker/               # Thư mục chứa các file liên quan đến MQTT broker
    ├── README.md              # Hướng dẫn cài đặt và sử dụng MQTT broker
    ├── install.sh             # Script cài đặt tự động
    ├── mosquitto.conf         # File cấu hình Mosquitto
    ├── mqtt-start.sh          # Script khởi động MQTT broker
    └── mqtt-stop.sh           # Script dừng MQTT broker
```

## Cảm biến được sử dụng

- DHT11/DHT22 (nhiệt độ và độ ẩm): kết nối với pin D5
- Cảm biến ánh sáng: kết nối với pin A0
- Cảm biến chuyển động PIR: kết nối với pin D4

## Cài đặt

### 1. Cài đặt MQTT Broker

Xem hướng dẫn chi tiết trong thư mục [mqtt-broker](mqtt-broker/README.md).

Cài đặt nhanh trên Ubuntu:
```bash
cd mqtt-broker
sudo ./install.sh
```

### 2. Cài đặt code cho NodeMCU ESP8266

1. Mở Arduino IDE
2. Cài đặt các thư viện cần thiết:
   - Vào "Sketch" > "Include Library" > "Manage Libraries..."
   - Tìm kiếm và cài đặt các thư viện sau:
     - "DHT sensor library" (thường là thư viện của Adafruit)
     - "PubSubClient" (thư viện MQTT cho ESP8266)
     - "ArduinoJson" (phiên bản 6.x)
3. Mở file `arduino-esp8266/arduino-esp8266.ino`
4. Cập nhật thông tin WiFi và MQTT broker:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";       // Thay đổi thành tên WiFi của bạn
   const char* password = "YOUR_WIFI_PASSWORD"; // Thay đổi thành mật khẩu WiFi của bạn
   const char* mqtt_server = "192.168.1.X"; // Thay X bằng địa chỉ IP thực tế của máy Ubuntu
   ```
5. Tải code lên NodeMCU ESP8266

### 3. Cài đặt API Server

1. Đảm bảo đã cài đặt Node.js (phiên bản 14.x trở lên) và npm
2. Di chuyển đến thư mục api-server:
   ```
   cd api-server
   ```
3. Cài đặt các phụ thuộc:
   ```
   npm install
   ```
4. Cấu hình file `.env` (đã được tự động cập nhật bởi script mqtt-start.sh)
5. Khởi động server:
   ```
   npm start
   ```

## API Endpoints

- `GET /api/sensors`: Lấy tất cả dữ liệu cảm biến
- `GET /api/status`: Kiểm tra trạng thái kết nối
- `GET /api/sensors/temperature`: Lấy dữ liệu nhiệt độ
- `GET /api/sensors/humidity`: Lấy dữ liệu độ ẩm
- `GET /api/sensors/light`: Lấy dữ liệu ánh sáng
- `GET /api/sensors/motion`: Lấy dữ liệu chuyển động
- `POST /api/simulation`: Bật/tắt chế độ giả lập

## Ví dụ phản hồi API

```json
// GET /api/sensors
{
  "temperature": 28.5,
  "humidity": 65.2,
  "light": 512,
  "motion": true,
  "timestamp": "2023-06-15T08:30:45.123Z",
  "isConnected": true
}
```

## Chế độ giả lập

Hệ thống hỗ trợ chế độ giả lập để kiểm thử API mà không cần kết nối với phần cứng thực tế. Để bật chế độ giả lập, đặt `SIMULATION_MODE=true` trong file `.env` hoặc sử dụng API:

```
POST /api/simulation
Content-Type: application/json

{
  "enabled": true
}
```

## Xử lý sự cố

1. **Không thể kết nối với MQTT broker**:
   - Kiểm tra xem MQTT broker đã được cài đặt và đang chạy
   - Kiểm tra cấu hình MQTT trong file `.env` và arduino-esp8266.ino
   - Kiểm tra tường lửa có chặn cổng 1883 không

2. **NodeMCU không kết nối được với WiFi**:
   - Kiểm tra thông tin WiFi trong arduino-esp8266.ino
   - Đảm bảo NodeMCU nằm trong phạm vi của WiFi

3. **Lỗi khi cài đặt các phụ thuộc Node.js**:
   - Đảm bảo đã cài đặt Node.js phiên bản mới nhất
   - Thử xóa thư mục node_modules và file package-lock.json, sau đó chạy lại `npm install`
