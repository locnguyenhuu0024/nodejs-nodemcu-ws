#!/bin/bash

# Script cài đặt MQTT broker (Mosquitto) tự động
# Chạy với quyền sudo: sudo ./install.sh

# Phát hiện hệ điều hành
OS_TYPE=$(uname -s)

# Kiểm tra quyền sudo
if [ "$EUID" -ne 0 ]; then
  echo "Vui lòng chạy script với quyền sudo: sudo ./install.sh"
  exit 1
fi

echo "=== Cài đặt MQTT Broker (Mosquitto) ==="
echo ""
echo "Hệ điều hành phát hiện: $OS_TYPE"
echo ""

# Cài đặt Mosquitto dựa trên hệ điều hành
echo "1. Cài đặt Mosquitto và các công cụ..."

if [ "$OS_TYPE" = "Darwin" ]; then
  # macOS
  if ! command -v brew &> /dev/null; then
    echo "Homebrew không được cài đặt. Đang cài đặt Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  fi
  
  brew install mosquitto
  
  # Tạo thư mục cấu hình nếu chưa tồn tại
  CONFIG_DIR=$(brew --prefix)/etc/mosquitto
  mkdir -p $CONFIG_DIR
  
  # Sao chép file cấu hình
  CONFIG_FILE="$CONFIG_DIR/mosquitto.conf"
  cp mosquitto.conf $CONFIG_FILE
  
  # Khởi động dịch vụ
  brew services start mosquitto
  
  echo "✓ Đã cài đặt và khởi động Mosquitto trên macOS"
else
  # Linux
  apt update
  apt install -y mosquitto mosquitto-clients
  
  # Sao chép file cấu hình
  CONFIG_DIR="/etc/mosquitto/conf.d"
  CONFIG_FILE="$CONFIG_DIR/default.conf"
  
  # Tạo thư mục nếu chưa tồn tại
  mkdir -p $CONFIG_DIR
  
  # Sao chép file cấu hình
  cp mosquitto.conf $CONFIG_FILE
  
  # Khởi động lại Mosquitto
  systemctl restart mosquitto
  
  # Kích hoạt tự động khởi động
  systemctl enable mosquitto
  
  echo "✓ Đã cài đặt Mosquitto trên Linux"
fi

echo ""

# Cấu hình tường lửa
echo "2. Cấu hình tường lửa để mở cổng MQTT (1883)..."

if [ "$OS_TYPE" = "Darwin" ]; then
  # macOS
  echo "Đang cấu hình tường lửa macOS..."
  
  # Kiểm tra xem ứng dụng có trong danh sách tường lửa chưa
  MOSQUITTO_PATH=$(which mosquitto)
  
  # Thêm quy tắc tường lửa cho Mosquitto
  /usr/libexec/ApplicationFirewall/socketfilterfw --add "$MOSQUITTO_PATH"
  /usr/libexec/ApplicationFirewall/socketfilterfw --unblockapp "$MOSQUITTO_PATH"
  
  # Mở cổng 1883 trên macOS
  echo "Mở cổng 1883 trên macOS có thể yêu cầu cấu hình thủ công."
  echo "Vui lòng vào System Preferences > Security & Privacy > Firewall > Firewall Options"
  echo "và đảm bảo Mosquitto được phép nhận kết nối đến."
  
  echo "✓ Đã cấu hình tường lửa macOS"
else
  # Linux - Kiểm tra và cấu hình UFW
  if command -v ufw &> /dev/null; then
    if ufw status | grep -q "active"; then
      echo "Đang cấu hình UFW..."
      ufw allow 1883/tcp
      echo "✓ Đã mở cổng 1883 trong UFW"
    else
      echo "UFW được cài đặt nhưng không hoạt động. Bỏ qua cấu hình tường lửa."
    fi
  # Linux - Kiểm tra và cấu hình firewalld
  elif command -v firewall-cmd &> /dev/null; then
    if systemctl is-active --quiet firewalld; then
      echo "Đang cấu hình firewalld..."
      firewall-cmd --permanent --add-port=1883/tcp
      firewall-cmd --reload
      echo "✓ Đã mở cổng 1883 trong firewalld"
    else
      echo "firewalld được cài đặt nhưng không hoạt động. Bỏ qua cấu hình tường lửa."
    fi
  else
    echo "Không phát hiện tường lửa UFW hoặc firewalld. Bỏ qua cấu hình tường lửa."
  fi
fi

echo ""

# Hiển thị thông tin
echo "=== Cài đặt hoàn tất ==="
echo ""
echo "MQTT broker đang chạy trên cổng 1883"
echo ""
echo "Địa chỉ IP của máy tính:"
if [ "$OS_TYPE" = "Darwin" ]; then
  # macOS
  ifconfig | grep "inet " | grep -v 127.0.0.1
else
  # Linux
  ip addr show | grep "inet " | grep -v 127.0.0.1
fi
echo ""

if [ "$OS_TYPE" = "Darwin" ]; then
  # macOS
  echo "Kiểm tra trạng thái: brew services info mosquitto"
  echo "Khởi động: brew services start mosquitto"
  echo "Dừng: brew services stop mosquitto"
else
  # Linux
  echo "Kiểm tra trạng thái: systemctl status mosquitto"
  echo "Khởi động: systemctl start mosquitto"
  echo "Dừng: systemctl stop mosquitto"
fi

echo ""
echo "Kiểm tra kết nối MQTT:"
echo "- Đăng ký nhận tin nhắn: mosquitto_sub -h localhost -t test/topic -v"
echo "- Gửi tin nhắn: mosquitto_pub -h localhost -t test/topic -m \"Hello MQTT\""
echo ""

# Lời khuyên bổ sung
echo "=== Lời khuyên bổ sung ==="
echo ""
echo "1. Nếu bạn vẫn gặp vấn đề kết nối, hãy kiểm tra:"
echo "   - Mosquitto đang chạy (sử dụng lệnh kiểm tra trạng thái ở trên)"
echo "   - Cổng 1883 đang mở (sử dụng: netstat -an | grep 1883)"
echo "   - Không có ứng dụng nào khác đang sử dụng cổng 1883"
echo ""
echo "2. Để kiểm tra tường lửa:"
if [ "$OS_TYPE" = "Darwin" ]; then
  # macOS
  echo "   - Sử dụng: sudo lsof -i :1883"
  echo "   - Kiểm tra System Preferences > Security & Privacy > Firewall"
else
  # Linux với UFW
  if command -v ufw &> /dev/null; then
    echo "   - Sử dụng: sudo ufw status | grep 1883"
  # Linux với firewalld
  elif command -v firewall-cmd &> /dev/null; then
    echo "   - Sử dụng: sudo firewall-cmd --list-ports | grep 1883"
  fi
fi
echo ""
echo "3. Nếu sử dụng máy ảo hoặc container, hãy đảm bảo đã cấu hình chuyển tiếp cổng."
echo "" 