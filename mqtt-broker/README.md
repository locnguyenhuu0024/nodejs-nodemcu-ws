# Hướng dẫn cài đặt và sử dụng MQTT Broker

## Cài đặt MQTT Broker (Mosquitto) trên Ubuntu

```bash
sudo apt update
sudo apt install -y mosquitto mosquitto-clients
```

## Cấu hình Mosquitto

1. Tạo file cấu hình:

```bash
sudo nano /etc/mosquitto/conf.d/default.conf
```

2. Thêm nội dung sau:

```
# Lắng nghe trên tất cả các giao diện mạng
listener 1883

# Cho phép kết nối không xác thực
allow_anonymous true

# Cấu hình cơ bản
persistence true
persistence_location /var/lib/mosquitto/
log_dest file /var/log/mosquitto/mosquitto.log
```

3. Khởi động lại Mosquitto:

```bash
sudo systemctl restart mosquitto
```

## Quản lý MQTT Broker

### Sử dụng script

- Khởi động: `./mqtt-start.sh`
- Dừng: `./mqtt-stop.sh`

### Sử dụng lệnh systemctl

- Khởi động: `sudo systemctl start mosquitto`
- Dừng: `sudo systemctl stop mosquitto`
- Kiểm tra trạng thái: `sudo systemctl status mosquitto`
- Kích hoạt tự động khởi động: `sudo systemctl enable mosquitto`
- Vô hiệu hóa tự động khởi động: `sudo systemctl disable mosquitto`

## Kiểm tra MQTT Broker

1. Đăng ký nhận tin nhắn:

```bash
mosquitto_sub -h localhost -t test/topic -v
```

2. Gửi tin nhắn (trong terminal khác):

```bash
mosquitto_pub -h localhost -t test/topic -m "Hello MQTT"
```

## Cấu hình tường lửa

Nếu bạn muốn kết nối từ các thiết bị khác trong mạng, hãy mở cổng 1883:

```bash
sudo ufw allow 1883
```

## Tìm địa chỉ IP của máy tính

```bash
ip addr show | grep "inet " | grep -v 127.0.0.1
```

## Cấu hình NodeMCU

Cập nhật địa chỉ IP của máy tính Ubuntu trong file arduino-esp8266.ino:

```cpp
const char* mqtt_server = "192.168.1.X"; // Thay X bằng địa chỉ IP thực tế của máy Ubuntu
```

## Cấu hình API Server

Cập nhật file .env để sử dụng broker cục bộ:

```
MQTT_BROKER=mqtt://localhost:1883
```

## Xử lý sự cố

1. **Không thể kết nối từ NodeMCU**:
   - Kiểm tra địa chỉ IP của máy tính Ubuntu
   - Kiểm tra tường lửa (ufw)
   - Đảm bảo NodeMCU và máy tính Ubuntu nằm trong cùng một mạng

2. **Lỗi khi khởi động Mosquitto**:
   - Kiểm tra log: `sudo tail -f /var/log/mosquitto/mosquitto.log`
   - Kiểm tra cấu hình: `sudo mosquitto -c /etc/mosquitto/mosquitto.conf -v` 