#!/bin/bash

echo "Dừng MQTT broker (Mosquitto)..."
sudo systemctl stop mosquitto
sleep 2

# Kiểm tra trạng thái
status=$(sudo systemctl is-active mosquitto)
if [ "$status" = "inactive" ]; then
    echo "MQTT broker đã dừng."
else
    echo "Lỗi: Không thể dừng MQTT broker."
    echo "Kiểm tra lỗi với lệnh: sudo systemctl status mosquitto"
fi 