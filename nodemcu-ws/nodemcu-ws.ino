#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Declare
int led = D1;

// WiFi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// WebSocket server
const char* ws_server = WS_SERVER;
const int ws_port = WS_PORT;

WebSocketsClient webSocket;
bool isConnected = false;
unsigned long lastMsg = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    StaticJsonDocument<200> doc;
    String message;

    switch(type) {
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
            
            message = "";
            serializeJson(doc, message);
            webSocket.sendTXT(message);
            break;
            
        case WStype_TEXT:
            Serial.printf("WebSocket received: %s\n", payload);
            
            // Parse JSON command
            doc.clear();
            DeserializationError error = deserializeJson(doc, payload);
            
            if (!error) {
                const char* command = doc["command"];
                const char* value = doc["value"];
                
                // Xử lý lệnh điều khiển LED
                if (strcmp(command, "LED_CONTROL") == 0) {
                    if (strcmp(value, "ON") == 0) {
                        digitalWrite(led, HIGH);
                        Serial.println("LED turned ON");
                    } else if (strcmp(value, "OFF") == 0) {
                        digitalWrite(led, LOW);
                        Serial.println("LED turned OFF");
                    }
                }
            }
            break;
    }
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    
    // Cấu hình WebSocket
    webSocket.begin(ws_server, ws_port, "/");
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
    
    pinMode(led, OUTPUT);
}

void loop() {
    webSocket.loop();

    // Gửi dữ liệu sensor định kỳ
    if (isConnected) {
        unsigned long now = millis();
        if (now - lastMsg > 5000) {
            lastMsg = now;
            
            // Tạo JSON message
            StaticJsonDocument<200> doc;
            doc["type"] = "sensor";
            doc["temperature"] = 25.5;  // Giả lập dữ liệu
            
            String message;
            serializeJson(doc, message);
            webSocket.sendTXT(message);
        }
    }
}