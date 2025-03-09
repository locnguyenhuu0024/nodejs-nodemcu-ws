#ifndef CONFIG_H
#define CONFIG_H

/*
 * Cấu hình mẫu cho Arduino ESP8266 WebSocket Client
 * 
 * Hướng dẫn:
 * 1. Sao chép file này thành config.h
 * 2. Thay đổi các thông số WiFi và server theo môi trường của bạn
 * 3. Nếu gặp vấn đề với DNS, hãy sử dụng địa chỉ IP thay vì tên miền
 * 4. Đảm bảo server WebSocket đang chạy và lắng nghe ở cổng đã cấu hình
 */

// WiFi credentials - Thay đổi thông tin WiFi của bạn
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"

// WebSocket server config - Thay đổi thông tin server của bạn
#define WS_SERVER "192.168.1.100"  // Địa chỉ IP hoặc tên miền của server WebSocket
// Đảm bảo cổng này khớp với cổng server trong server.js hoặc .env
// Kiểm tra file .env trong thư mục api-server
// QUAN TRỌNG: WS_PORT phải là chuỗi (string), không phải số
#define WS_PORT "3000"

// Cấu hình Serial
#define SERIAL_BAUD 115200         // Tốc độ Serial Monitor

// Cấu hình chân cảm biến
#define DHTPIN D5                  // Chân kết nối DHT
#define DHTTYPE DHT11              // Loại DHT (DHT11 hoặc DHT22)
#define LIGHT_SENSOR A0            // Chân kết nối cảm biến ánh sáng
#define PIR_SENSOR D4              // Chân kết nối cảm biến chuyển động PIR

// Cấu hình thời gian
#define SEND_INTERVAL 2000         // Gửi dữ liệu mỗi 2 giây
#define RECONNECT_INTERVAL 5000    // Thử kết nối lại mỗi 5 giây
#define CONNECT_TIMEOUT 10000      // Timeout kết nối (10 giây)

/*
 * Gỡ lỗi kết nối WebSocket:
 * 
 * Nếu bạn gặp vấn đề với việc kết nối WebSocket:
 * 
 * 1. Kiểm tra cổng WS_PORT có khớp với cổng server không
 *    - Xem trong file .env của server (PORT=3000)
 *    - WS_PORT phải được định nghĩa là chuỗi (string) với dấu ngoặc kép: "3000"
 * 
 * 2. Kiểm tra kết nối WiFi
 *    - Đảm bảo thông tin SSID và mật khẩu chính xác
 *    - Kiểm tra ESP8266 có thể kết nối được với mạng WiFi không
 * 
 * 3. Kiểm tra địa chỉ IP server
 *    - Đảm bảo địa chỉ IP server là chính xác
 *    - Thử ping địa chỉ IP từ máy tính để xác nhận kết nối
 */

#endif 