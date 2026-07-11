#include <Arduino.h>        // Là thư viện của framework arduino, khai báo để sử dụng các lệnh cơ bản được cung cấp sẵn
#include "network_config.h"

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

void setup() {
  
  //Chạy 1 lần duy nhất khi cấp điện cho mạch
  Serial.begin(115200);                           //Mở kênh giao tiếp với máy tính ở tốc độ 115200
  setup_network();                                //Khởi tạo gói mạng (Wifi + Server), có cơ chế reconnect được đóng gói trong network_config.h
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



//==============================****Q&A****==============================

/* 1. Publisher và Subscriber trong MQTT có nhất thiết phải là các mạch vi điều khiển giống như ESP32 không?
Không. Trong kiến trúc MQTT, người ta gọi chung các đối tượng tham gia là MQTT Client.
Client này có thể linh động là phần cứng nhúng (hardware client) như ESP32 và cũng có thể là phần mềm như NODE-RED trong project này.
=> Giao thức MQTT kết nối linh hoạt giữa phần cứng và phần mềm thông qua trạm trung chuyển Broker
*/