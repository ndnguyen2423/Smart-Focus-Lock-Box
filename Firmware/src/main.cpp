#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Cấu hình mạng Wifi (Khi nào ra quán cf hoặc lên trường thì đổi thông số này)
const char* ssid = "WIFI_NAME_KHA_DI";
const char* password = "WIFI_PASSWORD";

// Cấu hình MQTT Broker trung gian (HiveMQ miễn phí)
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Hàm kết nối Wifi hệ thống
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
  Serial.println("\nWiFi connected!");
}

// Hàm lắng nghe lệnh MQTT từ Node-RED đổ xuống
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  Serial.println(messageTemp);

  // ÔNG SẼ VIẾT LOGIC ĐIỀU KHIỂN SERVO CỦA ÔNG Ở ĐÂY
  // Ví dụ: if(messageTemp == "MO_KHOA") { // xoay servo }
}

// Hàm duy trì kết nối với trạm trung chuyển MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Tạo một ID ngẫu nhiên cho ESP32
    String clientId = "ESP32Client-" + String(random(0, 1000));
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
      // ĐĂNG KÝ CÁC TOPIC CẦN LẮNG NGHE Ở ĐÂY
      client.subscribe("team/dieukhien"); // Lắng nghe lệnh mở khóa của ông
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // THÀNH VIÊN 1 VÀ 3 SẼ THÊM CÁC HÀM pinMode() CỦA HỌ VÀO ĐÂY SAU
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // ĐÂY LÀ KHÔNG GIAN ĐỂ MERGE CODE ĐỌC CẢM BIẾN KHI OFFLINE
}
