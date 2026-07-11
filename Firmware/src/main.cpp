#include <Arduino.h>        // Là thư viện của framework arduino, khai báo để sử dụng các lệnh cơ bản được cung cấp sẵn
#include <WiFi.h>           // Thư viện giúp ESP32 bật ăng ten, dò và kết nối wifi, có sẵn trong framework arduino
#include <PubSubClient.h>   // Thư viện cài bên ngoài được set up trong platformio.ini, dùng để giao tiếp MQTT

//MQTT là protocol giao tiếp giữa các thiết bị điện tử với nhau
//Broker là server trung gian nhận tin từ publisher và gửi đi cho subscriber
//HiveMQ là nền tảng cung cấp Broker miễn phí cho cộng đồng mà project này xài

//Publisher và Subscriber trong MQTT có nhất thiết phải là các mạch vi điều khiển giống như ESP32 không?
/* Không. Trong kiến trúc MQTT, người ta gọi chung các đối tượng tham gia là MQTT Client.
Client này có thể linh động là phần cứng nhúng (hardware client) như ESP32 và cũng có thể là phần mềm như NODE-RED trong project này.
 => Giao thức MQTT kết nối linh hoạt giữa phần cứng và phần mềm thông qua trạm trung chuyển Broker
*/

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
}


/**
 * HÀM CALLBACK (HÀM HỨNG DỮ LIỆU TỪ MẠNG - EVENT-DRIVEN)
 * * 1. BẢN CHẤT HOẠT ĐỘNG (HẬU TRƯỜNG):
 * - Hàm này KHÔNG TỰ CHẠY và bạn không bao giờ gọi lệnh callback() trong code.
 * - Nó hoạt động dựa trên cơ chế Con trỏ hàm (Function Pointer). Khi bạn khai báo 
 * "client.setCallback(callback);" ở hàm setup(), bạn đang đưa địa chỉ của hàm này cho thư viện MQTT giữ.
 * - Hàng mili-giây, lệnh "client.loop()" ở hàm loop chính sẽ quét mạng. Khi phát hiện gói tin mới,
 * chính lệnh client.loop() sẽ lôi cái hàm callback này ra chạy và tự "nhét" dữ liệu vào 3 tham số bên dưới.
 * (Nếu xóa client.loop() ở hàm loop, hàm callback này sẽ BỊ LIỆT hoàn toàn).
 * * 2. TẠI SAO LẠI NHẬN CHÍNH XÁC 3 THAM SỐ NÀY?
 * - Giao thức MQTT quy định khi nhận tin nhắn bắt buộc phải biết: Nhận ở đâu (Topic nào) và Nội dung là gì (Payload là chuỗi các số 01).
 * - Do đặc thù C++ quản lý bộ nhớ rất nghiêm ngặt, tác giả thư viện phải chia làm 3 tham số như sau:
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    //Convert 0101010101 thành kí tự (char), sau đó được nối vào nessageTemp
    //payload là 1 chuỗi byte, mỗi phần tử là 1 byte (8bits) => range value [0;255]
    messageTemp += (char)payload[i];
  }
  Serial.println(messageTemp);

  // VIẾT LOGIC ĐIỀU KHIỂN SERVO
  // Ví dụ: if(messageTemp == "MO_KHOA") { // xoay servo }
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

void setup() {
  
  //Chạy 1 lần duy nhất khi cấp điện cho mạch
  Serial.begin(115200);                           //Mở kênh giao tiếp với máy tính ở tốc độ 115200
  setup_wifi();                                   //Gọi hàm bật wifi
  client.setServer(mqtt_server, mqtt_port);       //Khai báo địa chỉ server mà xài ké
  client.setCallback(callback);                   //Khai báo hàm callback để hứng tin nhắn, hàm callback này tự động chạy khi gọi client.loop trong void loop()
  
  // THÀNH VIÊN 1 VÀ 3 SẼ THÊM CÁC HÀM pinMode() CỦA HỌ VÀO SAU ĐÂY
}

void loop() {

  //Vì trong hàm setup chưa có khai báo bất kì lệnh kết nối MQTT nên chắc chắn khi chạy vào hàm loop sẽ vào hàm reconnected ít nhất 1 lần.
  if (!client.connected()) {
    reconnect();
  }

  /*client.loop(), loop() là hàm có sẵn của thư viện PubSubClient có tác dụng:
    1. Hứng gói tin: Liên tục kiểm tra xem bộ đệm mạng của ESP32 có gói tin nào gửi xuống không, nếu có thì chuyển vào callback để callback xử lí.
       Rõ hơn là ESP32 có 2 ông thợ, 1 là hệ điều hành mạng chạy ngầm trong chip (IwIP) và 1 là thợ chính (client.loop trong hàm void loop).
       Khi có dữ liệu tới thì ông IwIP sẽ gom toàn bộ thành 1 gói tin hoàn chỉnh trong RAM ở 1 phân vùng gọi là TCP Rx Buffer.
       Bộ đệm này trong RAM nhưng code ở hàm void loop không hề biết đến sự tồn tại của nó. Gói tin sẽ nằm im chờ xếp hàng để được xử lí.
       Khi hàm loop chạy đến client.loop thì ông thợ chính mới kiểm tra TCP Rx Buffer và bóc tách gói tín đó ra, chuyển cho hàm callback xử lí logic.
       Xử lí xong gói tin thì client.loop kiểm tra xem còn gói tin nào trong buffer để xử lí tiếp không? Nếu không thì nhường cho các câu lệnh khác trong void loop()

    2. Giữ liên lạc: MQTT quy định là nếu thiết bị im lặng quá lâu, server sẽ mặc định là thiết bị đã chết và ngắt kết nối.
       Hàm client.loop() sẽ tự động gửi 1 gói tin cực nhỏ (ping request) lên HiveMQ để báo là thiết bị vẫn còn sống.
    3. Giải phóng bộ đệm: Nếu có gửi dữ liệu lên (client.publish), hàm sẽ đảm bảo gói tin được đẩy đi hoàn toàn khỏi chip.
       Vì thiết lập mạng của đồ án là TCP -> Có bắt tay 3 bước. Khi publisher 1 gói tin thì phải tạo ra 1 bản sao của gói tin đó.
       Phòng trường hợp là gửi thất bại thì gửi lại. Nếu không có client.loop() để confirm ACK từ server trả về thì gói tin kẹt lại mãi.
       Nếu tiếp tục gửi gói tin khác thì hàng chờ bị đầy.

  */
  client.loop();

}
