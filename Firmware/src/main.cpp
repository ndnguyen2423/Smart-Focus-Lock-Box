#include <Arduino.h>        // Là thư viện của framework arduino, khai báo để sử dụng các lệnh cơ bản được cung cấp sẵn
#include <WiFi.h>           // Thư viện giúp ESP32 bật ăng ten, dò và kết nối wifi, có sẵn trong framework arduino
#include <PubSubClient.h>   // Thư viện cài bên ngoài được set up trong platformio.ini, dùng để giao tiếp MQTT

// Cấu hình mạng Wifi (Bạn sửa lại thành tên và pass WiFi ở nhà bạn hoặc điện thoại phát ra)
const char* ssid = "WIFI_NAME_KHA_DI";
const char* password = "WIFI_PASSWORD";

// Cấu hình MQTT Broker trung gian (HiveMQ miễn phí)
// Broker giống như cái bưu điện trung tâm, nhận tin nhắn từ Node-RED và phát về ESP32
const char* mqtt_server = "broker.hivemq.com";

// Cổng giao tiếp tiêu chuẩn của MQTT không mã hóa
const int mqtt_port = 1883;

//Wificlient là 1 class trong thư viện Wifi.h, bản chất là lớp kế thừa từ lớp Client của arduino, dùng để tạo ra 1 TCP SOCKET CLIENT
//Không phải là HTTP Client vì truyền xong tự ngắt kết nối. Không phải là UDP vì UDP dễ rớt gói (TCP có bắt tay 3 bước và luôn giữ kết nối)

//PubSubClient là 1 class trong PubSubClient.h, chịu trách nhiệm biên dịch MQTT thành các byte quy chuẩn, dựa vào espClient truyền tin

// Tạo ra 2 "người thợ" để làm việc:
WiFiClient espClient;           // Thợ 1: Phụ trách mảng mạng Internet (WiFi)
PubSubClient client(espClient); // Thợ 2: Phụ trách mảng nhắn tin MQTT, sử dụng đường truyền của Thợ 1


// Hàm kết nối Wifi hệ thống
void setup_wifi() {
  delay(10);                                      //Delay 10/1000s để cho ổn định hệ thống
  Serial.println();                               //In một dòng trống ở Serial Monitor để dễ tracking
  Serial.print("Connecting to ");                 //In ra dòng chữ Connecting to
  Serial.println(ssid);                           //In ra ssid, ssid là tên wifi đang cố kết nối đã khai báo ở trên


  /**
   * BƯỚC 1: BẬT CÔNG TẮC PHẦN CỨNG
   * Lệnh này chỉ làm ĐÚNG 1 NHIỆM VỤ: Ra lệnh cho chip WiFi phụ bật nguồn và đi dò mạng.
   * CPU chạy lệnh này xong mất vài phần triệu giây là nhảy xuống dòng dưới ngay, 
   * CHỨ KHÔNG ĐỢI WiFi kết nối xong (Bất đồng bộ - Asynchronous).
   */
  WiFi.begin(ssid, password);                     //Lệnh bắt đầu kết nối với tên wifi và mật khẩu wifi đã cài mặc định sẵn ở trên


  /**
   * BƯỚC 2: TRẠM KIỂM SOÁT (VÒNG LẶP KHÓA CHÂN CPU)
   * - WiFi.status() LÀ MỘT HÀM, không phải biến số! Mỗi lần vòng lặp chạy lại, 
   * CPU bắt buộc phải chạy vào hàm này để "ngó" trực tiếp xuống thanh ghi phần cứng xem chip WiFi làm ăn đến đâu rồi.
   * - WL_CONNECTED là hằng số trạng thái "Đã kết nối và có IP".
   * - Chừng nào trạng thái trả về KHÁC WL_CONNECTED, CPU sẽ bị giữ chân (nhốt) bên trong vòng while này.
   */
  while (WiFi.status() != WL_CONNECTED) {
    // CPU tạm ngủ 0.5 giây để tránh quá tải (giảm nhiệt cho chip).
    // QUAN TRỌNG: Trong lúc CPU đang ngủ, module phần cứng WiFi VẪN ĐANG CHẠY ĐỘC LẬP bên dưới 
    // để gửi gói tin bắt tay với Router và xin cấp IP.
    delay(500);

    // In ra dấu chấm trên Serial Monitor để lập trình viên biết mạch đang xử lý, không bị treo.
    Serial.print(".");

    /**
     * CƠ CHẾ THOÁT LẶP NGẦM:
     * Vào một thời điểm (ví dụ giây thứ 3), module WiFi phần cứng tự kết nối thành công, 
     * nó sẽ TỰ ĐỘNG GHI ĐÈ trạng thái WL_CONNECTED vào thanh ghi hệ thống của nó.
     * Ở lần lặp tiếp theo, hàm WiFi.status() đọc được giá trị mới này -> Điều kiện while SAI -> CPU được THOÁT ra ngoài!
     */
  }
  
  // CPU thoát khỏi vòng while nghĩa là mạng đã thông, sẵn sàng chạy các lệnh MQTT phía sau.
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
