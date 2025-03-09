#!/bin/bash

# Script cài đặt MQTT broker (Mosquitto) tự động
# Chạy với quyền sudo: sudo ./install.sh

# Kiểm tra quyền sudo
if [ "$EUID" -ne 0 ]; then
  echo "Vui lòng chạy script với quyền sudo: sudo ./install.sh"
  exit 1
fi

echo "=== Cài đặt MQTT Broker (Mosquitto) ==="
echo ""

# Cài đặt Mosquitto và các công cụ
echo "1. Cài đặt Mosquitto và các công cụ..."
apt update
apt install -y mosquitto mosquitto-clients
echo "✓ Đã cài đặt Mosquitto"
echo ""

# Sao chép file cấu hình
echo "2. Cấu hình Mosquitto..."
CONFIG_DIR="/etc/mosquitto/conf.d"
CONFIG_FILE="$CONFIG_DIR/default.conf"

# Tạo thư mục nếu chưa tồn tại
mkdir -p $CONFIG_DIR

# Sao chép file cấu hình
cp mosquitto.conf $CONFIG_FILE
echo "✓ Đã sao chép file cấu hình vào $CONFIG_FILE"
echo ""

# Khởi động lại Mosquitto
echo "3. Khởi động lại Mosquitto..."
systemctl restart mosquitto
echo "✓ Đã khởi động lại Mosquitto"
echo ""

# Kích hoạt tự động khởi động
echo "4. Kích hoạt tự động khởi động..."
systemctl enable mosquitto
echo "✓ Đã kích hoạt tự động khởi động"
echo ""

# Mở cổng tường lửa nếu UFW được kích hoạt
if command -v ufw &> /dev/null && ufw status | grep -q "active"; then
  echo "5. Cấu hình tường lửa (UFW)..."
  ufw allow 1883/tcp
  echo "✓ Đã mở cổng 1883 trong tường lửa"
  echo ""
fi

# Hiển thị thông tin
echo "=== Cài đặt hoàn tất ==="
echo ""
echo "MQTT broker đang chạy trên cổng 1883"
echo ""
echo "Địa chỉ IP của máy tính:"
ip addr show | grep "inet " | grep -v 127.0.0.1
echo ""
echo "Kiểm tra trạng thái: systemctl status mosquitto"
echo "Khởi động: systemctl start mosquitto"
echo "Dừng: systemctl stop mosquitto"
echo ""
echo "Kiểm tra kết nối MQTT:"
echo "- Đăng ký nhận tin nhắn: mosquitto_sub -h localhost -t test/topic -v"
echo "- Gửi tin nhắn: mosquitto_pub -h localhost -t test/topic -m \"Hello MQTT\""
echo "" 