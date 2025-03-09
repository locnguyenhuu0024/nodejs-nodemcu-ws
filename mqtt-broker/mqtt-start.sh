#!/bin/bash

echo "Khởi động MQTT broker (Mosquitto)..."
sudo systemctl start mosquitto
sleep 2

# Kiểm tra trạng thái
status=$(sudo systemctl is-active mosquitto)
if [ "$status" = "active" ]; then
    echo "MQTT broker đang chạy."
    echo "Địa chỉ IP của máy tính:"
    ip addr show | grep "inet " | grep -v 127.0.0.1
    echo ""
    echo "Cổng MQTT: 1883"
    echo ""
    echo "Kiểm tra kết nối MQTT:"
    echo "Trong terminal khác, chạy: mosquitto_sub -h localhost -t test/topic -v"
    echo "Sau đó chạy: mosquitto_pub -h localhost -t test/topic -m \"Hello MQTT\""
    
    # Cập nhật file .env của API server
    if [ -f "../api-server/.env" ]; then
        echo ""
        echo "Cập nhật file .env của API server..."
        # Lấy địa chỉ IP của máy tính
        ip_address=$(ip addr show | grep "inet " | grep -v 127.0.0.1 | awk '{print $2}' | cut -d/ -f1 | head -n1)
        if [ -n "$ip_address" ]; then
            echo "Địa chỉ IP: $ip_address"
            # Cập nhật file .env
            sed -i "s|MQTT_BROKER=.*|MQTT_BROKER=mqtt://localhost:1883|g" ../api-server/.env
            echo "Đã cập nhật file .env của API server."
        fi
    fi
else
    echo "Lỗi: MQTT broker không khởi động được."
    echo "Kiểm tra lỗi với lệnh: sudo systemctl status mosquitto"
fi 