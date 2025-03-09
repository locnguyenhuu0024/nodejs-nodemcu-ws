#ifndef CONFIG_H
#define CONFIG_H

/*
 * Cấu hình mẫu cho NodeMCU MQTT Publisher
 * 
 * Hướng dẫn:
 * 1. Sao chép file này thành config.h
 * 2. Thay đổi các thông số WiFi và MQTT broker theo môi trường của bạn
 * 3. Đảm bảo MQTT broker đang chạy và lắng nghe ở cổng đã cấu hình
 */

// WiFi credentials - Thay đổi thông tin WiFi của bạn
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"

// MQTT Broker config - Thay đổi thông tin MQTT broker của bạn
#define MQTT_SERVER "192.168.1.100"  // Địa chỉ IP hoặc tên miền của MQTT broker
#define MQTT_PORT 1883               // Cổng MQTT broker (thường là 1883)
#define MQTT_USER ""                 // Tên người dùng MQTT (để trống nếu không cần xác thực)
#define MQTT_PASSWORD ""             // Mật khẩu MQTT (để trống nếu không cần xác thực)
#define MQTT_TOPIC "sensors/data"    // Topic MQTT để publish dữ liệu

// WebSocket server config (nếu cần)
#define WS_SERVER "192.168.1.100"    // Địa chỉ IP hoặc tên miền của WebSocket server
#define WS_PORT 3000                 // Cổng WebSocket server

// Cấu hình chân cảm biến
#define DHTPIN D5                    // Chân kết nối DHT
#define DHTTYPE DHT11                // Loại DHT (DHT11 hoặc DHT22)
#define LIGHT_SENSOR A0              // Chân kết nối cảm biến ánh sáng
#define PIR_SENSOR D4                // Chân kết nối cảm biến chuyển động PIR

// Cấu hình thời gian
#define SEND_INTERVAL 2000           // Gửi dữ liệu mỗi 2 giây
#define RECONNECT_INTERVAL 5000      // Thử kết nối lại mỗi 5 giây

/*
 * Gỡ lỗi kết nối MQTT:
 * 
 * Nếu bạn gặp vấn đề với việc kết nối MQTT:
 * 
 * 1. Kiểm tra cổng MQTT_PORT có khớp với cổng broker không
 *    - Cổng mặc định là 1883, nhưng có thể thay đổi tùy theo cấu hình broker
 * 
 * 2. Kiểm tra kết nối WiFi
 *    - Đảm bảo thông tin SSID và mật khẩu chính xác
 *    - Kiểm tra NodeMCU có thể kết nối được với mạng WiFi không
 * 
 * 3. Kiểm tra địa chỉ IP broker
 *    - Đảm bảo địa chỉ IP broker là chính xác
 *    - Thử ping địa chỉ IP từ máy tính để xác nhận kết nối
 * 
 * 4. Kiểm tra xác thực MQTT
 *    - Nếu broker yêu cầu xác thực, đảm bảo MQTT_USER và MQTT_PASSWORD chính xác
 */

#endif 