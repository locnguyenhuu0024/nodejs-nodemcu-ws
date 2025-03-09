// Code đọc dữ liệu từ các cảm biến và gửi qua MQTT
// Sử dụng:
// - DHT: pin D5
// - Cảm biến ánh sáng: pin A0
// - Cảm biến có người ở gần (PIR): pin D4

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Cấu hình WiFi
const char* ssid = "Huu Loc";       // Thay đổi thành tên WiFi của bạn
const char* password = "locdeptraivodich"; // Thay đổi thành mật khẩu WiFi của bạn

// Cấu hình MQTT - Sử dụng broker cục bộ
// Để tìm địa chỉ IP của máy Ubuntu, chạy lệnh: ip addr show | grep "inet " | grep -v 127.0.0.1
const char* mqtt_server = "192.168.1.X"; // Thay X bằng địa chỉ IP thực tế của máy Ubuntu
//const char* mqtt_server = "broker.emqx.io"; // EMQX broker công cộng (dự phòng)
//const char* mqtt_server = "broker.hivemq.com"; // HiveMQ broker công cộng
//const char* mqtt_server = "test.mosquitto.org"; // Mosquitto broker công cộng
const int mqtt_port = 1883;
const char* mqtt_user = "";                // Để trống nếu không cần xác thực
const char* mqtt_password = "";            // Để trống nếu không cần xác thực
const char* mqtt_topic = "huuloc/sensors/data";   // Thay đổi topic để tránh xung đột

// Định nghĩa các chân kết nối
#define DHTPIN D5       // Chân kết nối DHT
#define DHTTYPE DHT11   // Loại DHT (DHT11 hoặc DHT22)
#define LIGHT_SENSOR A0 // Chân kết nối cảm biến ánh sáng
#define PIR_SENSOR D4   // Chân kết nối cảm biến chuyển động PIR

// Khởi tạo đối tượng WiFi, MQTT và DHT
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

// Biến lưu trữ thời gian
unsigned long lastSendTime = 0;
const long sendInterval = 2000; // Gửi dữ liệu mỗi 2 giây
unsigned long lastReconnectAttempt = 0;
const long reconnectInterval = 5000; // Thử kết nối lại MQTT mỗi 5 giây
unsigned long mqttConnectTimeout = 10000; // Timeout kết nối MQTT (10 giây)

void setup() {
  // Khởi tạo Serial với tốc độ baud 9600
  Serial.begin(9600);
  Serial.println("\n\n=== NodeMCU ESP8266 MQTT Sensor ===");
  
  // Khởi tạo DHT
  dht.begin();
  
  // Cấu hình chân PIR là INPUT
  pinMode(PIR_SENSOR, INPUT);
  
  // Kết nối WiFi
  setup_wifi();
  
  // Cấu hình MQTT
  client.setServer(mqtt_server, mqtt_port);
  // Đặt timeout kết nối MQTT
  client.setSocketTimeout(10); // 10 giây
  
  Serial.println("Khởi động hệ thống cảm biến...");
  delay(2000); // Chờ các cảm biến khởi động
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Đang kết nối đến WiFi: ");
  Serial.println(ssid);

  // Đặt chế độ station và ngắt kết nối hiện tại
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Bắt đầu kết nối
  WiFi.begin(ssid, password);

  // Đợi kết nối với timeout
  unsigned long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startAttemptTime < 20000) { // Timeout sau 20 giây
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("Đã kết nối WiFi");
    Serial.print("Địa chỉ IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Cường độ tín hiệu: ");
    Serial.println(WiFi.RSSI());
    
    // Kiểm tra kết nối Internet
    Serial.println("Kiểm tra kết nối đến MQTT broker...");
    if (testInternetConnection()) {
      Serial.println("Kết nối đến MQTT broker OK");
    } else {
      Serial.println("Không thể kết nối đến MQTT broker. Vui lòng kiểm tra địa chỉ IP và tường lửa.");
    }
  } else {
    Serial.println("");
    Serial.println("Không thể kết nối WiFi. Vui lòng kiểm tra SSID và mật khẩu.");
    Serial.println("Thiết bị sẽ khởi động lại sau 5 giây...");
    delay(5000);
    ESP.restart();
  }
}

// Kiểm tra kết nối Internet bằng cách ping đến broker MQTT
bool testInternetConnection() {
  IPAddress remote_addr;
  if (WiFi.hostByName(mqtt_server, remote_addr)) {
    Serial.print("Ping đến ");
    Serial.print(mqtt_server);
    Serial.print(" (");
    Serial.print(remote_addr);
    Serial.println(")");
    return true;
  }
  return false;
}

boolean reconnect() {
  Serial.print("Đang kết nối MQTT...");
  
  // Tạo ID client ngẫu nhiên để tránh xung đột
  String clientId = "NodeMCU-ESP8266-";
  clientId += String(random(0xffff), HEX);
  
  // Đặt timeout cho kết nối
  unsigned long startAttemptTime = millis();
  
  // Thử kết nối với MQTT broker
  if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
    Serial.println("đã kết nối");
    Serial.print("Client ID: ");
    Serial.println(clientId);
    Serial.print("Broker: ");
    Serial.println(mqtt_server);
    Serial.print("Topic: ");
    Serial.println(mqtt_topic);
    
    // Đăng ký nhận tin nhắn (nếu cần)
    // client.subscribe("inTopic");
    
    return true;
  } else {
    Serial.print("lỗi, rc=");
    Serial.print(client.state());
    
    // Giải thích mã lỗi
    switch(client.state()) {
      case -4: 
        Serial.println(" (MQTT_CONNECTION_TIMEOUT: Kết nối bị timeout)");
        break;
      case -3: 
        Serial.println(" (MQTT_CONNECTION_LOST: Mất kết nối)");
        break;
      case -2: 
        Serial.println(" (MQTT_CONNECT_FAILED: Kết nối thất bại)");
        break;
      case -1: 
        Serial.println(" (MQTT_DISCONNECTED: Client ngắt kết nối)");
        break;
      case 1: 
        Serial.println(" (MQTT_CONNECT_BAD_PROTOCOL: Phiên bản protocol không được hỗ trợ)");
        break;
      case 2: 
        Serial.println(" (MQTT_CONNECT_BAD_CLIENT_ID: ID client bị từ chối)");
        break;
      case 3: 
        Serial.println(" (MQTT_CONNECT_UNAVAILABLE: Server không khả dụng)");
        break;
      case 4: 
        Serial.println(" (MQTT_CONNECT_BAD_CREDENTIALS: Thông tin xác thực sai)");
        break;
      case 5: 
        Serial.println(" (MQTT_CONNECT_UNAUTHORIZED: Không được phép kết nối)");
        break;
      default:
        Serial.println(" (Lỗi không xác định)");
    }
    
    // Kiểm tra kết nối WiFi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Mất kết nối WiFi. Đang kết nối lại...");
      setup_wifi();
    } else {
      // Kiểm tra kết nối Internet
      if (!testInternetConnection()) {
        Serial.println("Không thể kết nối đến broker MQTT. Kiểm tra địa chỉ IP và tường lửa.");
      }
    }
    
    return false;
  }
}

void loop() {
  // Kiểm tra kết nối MQTT
  if (!client.connected()) {
    // Thử kết nối lại theo khoảng thời gian
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Xử lý các tin nhắn MQTT đến (nếu có)
    client.loop();
    
    // Gửi dữ liệu cảm biến theo khoảng thời gian
    unsigned long currentMillis = millis();
    if (currentMillis - lastSendTime >= sendInterval) {
      lastSendTime = currentMillis;
      sendSensorData();
    }
  }
}

void sendSensorData() {
  // Đọc giá trị nhiệt độ và độ ẩm từ DHT
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Đọc giá trị cảm biến ánh sáng
  int lightValue = analogRead(LIGHT_SENSOR);
  
  // Đọc trạng thái cảm biến PIR
  int pirState = digitalRead(PIR_SENSOR);
  bool motionDetected = (pirState == LOW); // LOW = có chuyển động (tùy thuộc vào loại cảm biến)
  
  // Kiểm tra xem việc đọc DHT có thành công không
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Lỗi đọc dữ liệu từ cảm biến DHT!");
    return;
  }
  
  // In thông tin ra Serial Monitor (để debug)
  Serial.println("=== THÔNG TIN CẢM BIẾN ===");
  Serial.print("Nhiệt độ: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("Độ ẩm: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  Serial.print("Ánh sáng: ");
  Serial.println(lightValue);
  
  Serial.print("Phát hiện chuyển động: ");
  Serial.println(motionDetected ? "Có" : "Không");
  
  Serial.println("=========================");
  
  // Tạo JSON để gửi qua MQTT
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;
  jsonDoc["light"] = lightValue;
  jsonDoc["motion"] = motionDetected;
  jsonDoc["timestamp"] = millis();
  
  char jsonBuffer[200];
  serializeJson(jsonDoc, jsonBuffer);
  
  // Gửi dữ liệu qua MQTT
  if (client.publish(mqtt_topic, jsonBuffer)) {
    Serial.println("Đã gửi dữ liệu qua MQTT");
  } else {
    Serial.println("Lỗi khi gửi dữ liệu qua MQTT");
  }
}
