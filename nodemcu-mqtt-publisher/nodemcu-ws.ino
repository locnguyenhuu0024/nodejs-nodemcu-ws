#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include <Ticker.h>

// Declare PIN
#define LED D0
#define HONGNGOAI D1
#define ANHSANG A0

// Declare value
int hongNgoai = LOW;
int anhSang = 0;
int troiToi = 600;
int statusLED = LOW;

// WiFi credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// WebSocket server
const char *ws_server = WS_SERVER;
const int ws_port = WS_PORT;
const char *ws_path = "/nodemcu"; // Đường dẫn WebSocket cho NodeMCU

WebSocketsClient webSocket;
bool isConnected = false;
unsigned long lastMsg = 0;

Ticker sensorTicker;
volatile int lastHongNgoai = LOW;
volatile int lastAnhSang = 0;
volatile bool hasNewData = false;

// Thêm biến để theo dõi trạng thái LED
bool ledState = false;

void setUpCamBien()
{
  pinMode(HONGNGOAI, INPUT);
  pinMode(ANHSANG, INPUT);
}

int readHongNgoai()
{
  return digitalRead(HONGNGOAI) == LOW ? HIGH : LOW; // có người thì 1, ngược lại là 0
}

int readAnhSang()
{
  int sum = 0;
  for (int i = 0; i < 5; i++)
  { // Đọc 5 lần và lấy trung bình
    sum += analogRead(ANHSANG);
    delay(1); // Delay nhỏ giữa các lần đọc
  }
  return sum / 5;
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  StaticJsonDocument<200> doc;
  String message;

  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("WebSocket Disconnected!");
    isConnected = false;
    break;

  case WStype_CONNECTED:
    Serial.println("WebSocket Connected!");
    isConnected = true;

    // Gửi status online
    doc.clear();
    doc["type"] = "status";
    doc["status"] = "online";
    doc["device"] = "nodemcu";
    doc["led"] = ledState ? "ON" : "OFF";

    message = "";
    serializeJson(doc, message);
    webSocket.sendTXT(message);
    break;

  case WStype_TEXT:
    Serial.printf("WebSocket received: %s\n", payload);

    // Parse JSON command
    doc.clear();
    DeserializationError error = deserializeJson(doc, payload);

    if (!error)
    {
      const char *command = doc["command"];
      
      // Xử lý lệnh điều khiển LED
      if (strcmp(command, "LED_CONTROL") == 0)
      {
        const char *value = doc["value"];
        if (strcmp(value, "ON") == 0)
        {
          digitalWrite(LED, HIGH);
          ledState = true;
          Serial.println("LED turned ON");
          
          // Gửi phản hồi về trạng thái LED
          doc.clear();
          doc["type"] = "led_status";
          doc["status"] = "ON";
          message = "";
          serializeJson(doc, message);
          webSocket.sendTXT(message);
        }
        else if (strcmp(value, "OFF") == 0)
        {
          digitalWrite(LED, LOW);
          ledState = false;
          Serial.println("LED turned OFF");
          
          // Gửi phản hồi về trạng thái LED
          doc.clear();
          doc["type"] = "led_status";
          doc["status"] = "OFF";
          message = "";
          serializeJson(doc, message);
          webSocket.sendTXT(message);
        }
      }
      // Xử lý lệnh GET_STATUS
      else if (strcmp(command, "GET_STATUS") == 0)
      {
        // Gửi trạng thái hiện tại
        doc.clear();
        doc["type"] = "status";
        doc["status"] = "online";
        doc["device"] = "nodemcu";
        doc["led"] = ledState ? "ON" : "OFF";
        
        message = "";
        serializeJson(doc, message);
        webSocket.sendTXT(message);
        
        // Gửi thêm dữ liệu cảm biến
        sendSensorData(lastHongNgoai, lastAnhSang);
      }
    }
    break;
  }
}

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); // Thêm mode
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); // Khởi tạo LED ở trạng thái tắt
  ledState = false;
  setUpCamBien();
  
  // Cấu hình WebSocket với đường dẫn mới
  webSocket.begin(ws_server, ws_port, ws_path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  // Thêm cấu hình heartbeat để giữ kết nối ổn định
  webSocket.enableHeartbeat(15000, 3000, 2);

  sensorTicker.attach(2, readSensors); // Đọc cảm biến mỗi 2 giây
  
  Serial.println("NodeMCU started, connecting to WebSocket server at path: " + String(ws_path));
}

void sendMessage(const char *type, const char *key, const char *value)
{
  if (!isConnected)
    return; // Không gửi nếu không có kết nối

  StaticJsonDocument<200> doc;
  String message;

  doc["type"] = type;
  doc[key] = value;
  doc["device"] = "nodemcu";

  serializeJson(doc, message);
  webSocket.sendTXT(message);
}

void sendSensorData(int hongNgoai, int anhSang)
{
  if (!isConnected)
    return;

  StaticJsonDocument<200> doc;
  String message;

  doc["type"] = "sensor_data";
  doc["device"] = "nodemcu";
  doc["motion"] = hongNgoai == HIGH ? "detected" : "none";
  doc["light"] = anhSang;
  doc["is_dark"] = anhSang > troiToi ? true : false;
  doc["led"] = ledState ? "ON" : "OFF";

  serializeJson(doc, message);
  webSocket.sendTXT(message);
}

void readSensors()
{
  lastHongNgoai = readHongNgoai();
  lastAnhSang = readAnhSang();
  hasNewData = true;
}

void loop()
{
  webSocket.loop();

  if (isConnected && hasNewData)
  {
    sendSensorData(lastHongNgoai, lastAnhSang);
    hasNewData = false;
    delay(10);
  }
  
  // Kiểm tra kết nối WiFi và kết nối lại nếu cần
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    setup_wifi();
  }
  
  yield();
}