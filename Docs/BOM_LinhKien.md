# Danh mục Vật tư và Thiết bị (BOM) - Dự án Smart Focus Lock Box

Hệ thống được thiết kế và thi công theo mô hình sản phẩm thật, tuân thủ nghiêm ngặt quy định không tái sử dụng cảm biến giữa các luồng độc lập của môn học.

## 1. Danh sách Linh kiện Điện tử và Cơ khí chi tiết

| STT | Tên sản phẩm | Số lượng | Đơn giá (Ước tính) | Tổng tiền | Trách nhiệm quản lý | Công dụng kỹ thuật trong hệ thống |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | Mạch vi điều khiển ESP32 DOIT DevKit V1 | 01 | 95.000 VNĐ | 95.000 VNĐ | Toàn nhóm | Bộ xử lý trung tâm; duy trì kết nối mạng WiFi và giao thức MQTT qua Broker HiveMQ; tiếp nhận dữ liệu Input và xuất lệnh điều khiển Output. |
| 2 | Động cơ Servo MG996R (Nhông kim loại, góc quay 180 độ) | 01 | 90.000 VNĐ | 90.000 VNĐ | Thành viên 2 (Trưởng nhóm) | Thiết bị Output 1; tiếp nhận dữ liệu điều khiển từ Node-RED qua topic `team/dieukhien` để xoay tay đòn 90 độ thực hiện chốt khóa. |
| 3 | Tay đòn Servo bằng nhôm CNC (Loại 25T) | 01 | 15.000 VNĐ | 15.000 VNĐ | Thành viên 2 (Trưởng nhóm) | Thay thế tay đòn nhựa mặc định; liên kết trực tiếp với móc chốt của nắp hộp để chịu lực kéo và chống cạy phá vật lý. |
| 4 | Mạch cảm biến chạm điện dung TTP223 | 01 | 5.000 VNĐ | 5.000 VNĐ | Thành viên 1 | Thiết bị Input 1; ghi nhận thao tác chạm của người dùng để truyền lệnh kích hoạt chu kỳ đếm ngược qua topic `team/khoa`. |
| 5 | Mạch cảm biến rung/góc nghiêng SW-420 | 01 | 15.000 VNĐ | 15.000 VNĐ | Thành viên 3 | Thiết bị Input 2; đặt bên trong khoang kỹ thuật để phát hiện ngoại lực bất thường (lắc, cạy), truyền lệnh cảnh báo qua topic `team/canhbao`. |
| 6 | Cuộn nhựa in 3D PLA Filament (Đường kính 1.75mm) | 0.5 kg | 90.000 VNĐ | 90.000 VNĐ | Toàn nhóm | Vật liệu chế tạo vỏ hộp đúc phẳng, vách ngăn phân lập khoang kỹ thuật và khoang chứa điện thoại, cấu trúc móc khóa chữ L trên nắp. |
| 7 | Bảng cắm mạch Breadboard (Loại 400 lỗ) | 01 | 20.000 VNĐ | 20.000 VNĐ | Toàn nhóm | Bo mạch cầu nối trung gian để định tuyến các đường nguồn và dây tín hiệu giữa chip ESP32 với các thiết bị ngoại vi. |
| 8 | Dây cắm DuPont (Jumper Wires) Đực-Đực và Đực-Cái | 02 bó | 10.000 VNĐ | 20.000 VNĐ | Toàn nhóm | Dẫn dòng điện và truyền tín hiệu logic giữa các chân GPIO của vi điều khiển và các module cảm biến/động cơ. |
| 9 | Củ sạc điện thoại nguồn DC 5V - 2A | 01 | 40.000 VNĐ | 40.000 VNĐ | Toàn nhóm | Cấp nguồn điện tổng cho hệ thống; dòng điện 2A đảm bảo chống hiện tượng sụt áp (Brownout Reset) khi động cơ Servo hoạt động hết tải. |
| 10 | Vật tư phụ phụ trợ (Ốc vít M3, keo nến cố định) | 1 túi | 10.000 VNĐ | 10.000 VNĐ | Toàn nhóm | Cố định chắc chắn thân động cơ Servo và các mạch cảm biến vào các ngàm giữ bên trong vỏ hộp in 3D để triệt tiêu phản lực cơ học. |
| 11 | Jack DC Cái | 1 cái | 10.000 VNĐ | 10.000 VNĐ | Toàn nhóm | dùng Jack DC cái để cắm đầu sạc vào, sau đó vặn vít cố định để chia dây nguồn ra Breadboard một cách an toàn, chắc chắn. |


## 2. Tổng kết Ngân sách Dự án
* **Tổng chi phí đầu tư thực tế:** **410.000 VNĐ** (Bằng chữ: Bốn trăm nghìn đồng chẵn).
* **Trạng thái ngân sách:** Hợp lệ (Nằm trong hạn mức kiểm soát tối đa 500.000 VNĐ của nhóm).
