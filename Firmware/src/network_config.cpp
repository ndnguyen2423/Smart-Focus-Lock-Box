#include "network_config.h"
#include <WiFi.h> // Thư viện giúp ESP32 bật ăng ten, dò và kết nối wifi, có sẵn trong framework arduino

// Cấu hình mạng Wifi (Bạn sửa lại thành tên và pass WiFi ở nhà bạn hoặc điện thoại phát ra)
const char* ssid = "WIFI_NAME_KHA_DI";
const char* password = "WIFI_PASSWORD";

//MQTT là protocol giao tiếp giữa các thiết bị điện tử với nhau
//Broker là server trung gian nhận tin từ publisher và gửi đi cho subscriber
//HiveMQ là nền tảng cung cấp Broker miễn phí cho cộng đồng mà project này xài
const char* mqtt_server = "broker.hivemq.com";

// Cổng giao tiếp tiêu chuẩn của MQTT không mã hóa
const int mqtt_port = 1883;


//Wificlient là 1 class trong thư viện Wifi.h, bản chất là lớp kế thừa từ lớp Client của arduino, dùng để tạo ra 1 TCP SOCKET CLIENT
//Không phải là HTTP Client vì truyền xong tự ngắt kết nối. Không phải là UDP vì UDP dễ rớt gói (TCP có bắt tay 3 bước và luôn giữ kết nối)

//PubSubClient là 1 class trong PubSubClient.h, chịu trách nhiệm biên dịch MQTT thành các byte quy chuẩn, dựa vào espClient truyền tin

// Tạo ra 2 "người thợ" để làm việc:
WiFiClient espClient;           // Thợ 1: Phụ trách mảng mạng Internet (WiFi)
PubSubClient client(espClient); // Thợ 2: Phụ trách mảng nhắn tin MQTT, sử dụng đường truyền của Thợ 1


// Hàm kết nối Wifi, và kết nối tới broker trên server
void setup_network() {
  delay(10);                                      //Delay 10/1000s để cho ổn định hệ thống
  Serial.println();                               //In một dòng trống ở Serial Monitor để dễ tracking
  Serial.print("Connecting to ");                 //In ra dòng chữ Connecting to
  Serial.println(ssid);                           //In ra ssid, ssid là tên wifi đang cố kết nối đã khai báo ở trên

  /**
   * BƯỚC 1: BẬT CÔNG TẮC PHẦN CỨNG
   * Wifi là một global object được thư viện Wifi.h cung cấp sẵn. Chỉ việc lôi ra xài, không cần Wifi Wifi; (khai báo)
   * Lệnh này chỉ làm ĐÚNG 1 NHIỆM VỤ: Ra lệnh cho chip WiFi phụ bật nguồn và đi dò mạng.
   * CPU chạy lệnh này xong mất vài phần triệu giây là nhảy xuống dòng dưới ngay, 
   * CHỨ KHÔNG ĐỢI WiFi kết nối xong => Dễ gây lỗi bất đồng bộ - Asynchronous.
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

  //Khai báo server đang xài là Broker (HiveMQ) với port 1883
  client.setServer(mqtt_server, mqtt_port);

}



// Internet sau khi kết nối 1 lần có thể chập chờn
// Hàm này đảm bảo là khi rớt internet thì ESP tự động kết nối lại được wifi, duy trì kết nối server HiveMQ được
void reconnect() {

  //Kiểm tra xem có kết nối được với MQTT chưa? Lưu ý hàm connected() là hàm kiểm tra => Trả về đúng sai, không cần tham số truyền vào
  //Nếu chưa kết nối thì liên tục cố gắng kết nối lại
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Tạo một ID ngẫu nhiên cho ESP32.
    // Quy định của Broker HiveMQ là mỗi thiết bị kết nối phải có 1 tên (ID) độc nhất. 
    // Nếu có 1 thiết bị trùng tên thì sẽ đá 1 thiết bị cũ ra
    String clientId = "SmartLockBox_FIT24_HCMUS_ESP32Client-" + String(random(0, 1000));


    //Đây là hàm connect chứ không phải connected, nên cần phải truyền tham số là clientId vào
    //Gửi lệnh kết nối với Broker HiveMQ bằng tên thiết bị là clientId này
    //Nếu thành công thì in ra connected, không thì in lỗi và kết nối lại sau 5 giây
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
      // ĐĂNG KÝ CÁC TOPIC CẦN LẮNG NGHE Ở ĐÂY
      // 1. Topic "SmartLockBox_FIT24_HCMUS/dieukhien" là chuỗi ký tự do nhóm TỰ QUY ĐỊNH.
      // 2. Trên HiveMQ KHÔNG CÓ SẴN luồng này. Khi ESP32 Subscribe, HiveMQ mới tạo động trong RAM.
      // 3. Dấu '/' dùng để phân cấp tính năng (dieukhien, cambien, pin) theo ý đồ của nhóm.
      // 4. HiveMQ hiểu cấu trúc '/' này để hỗ trợ truy xuất dữ liệu bằng ký tự đại diện '+' hoặc '#' sau này
      client.subscribe("SmartLockBox_FIT24_HCMUS/dieukhien"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
