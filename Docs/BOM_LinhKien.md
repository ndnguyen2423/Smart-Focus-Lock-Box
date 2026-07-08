# Danh mục Vật tư và Thiết bị (BOM) - Dự án Smart Focus Lock Box

Hệ thống được thiết kế và thi công theo mô hình sản phẩm thật, tuân thủ nghiêm ngặt quy định không tái sử dụng cảm biến giữa các luồng độc lập của môn học.

## 1. Danh sách Linh kiện Điện tử và Cơ khí chi tiết

| STT | Tên sản phẩm | Số lượng | Đơn giá (Ước tính) | Tổng tiền | Trách nhiệm quản lý | Công dụng kỹ thuật trong hệ thống |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | Mạch vi điều khiển ESP32 DOIT DevKit V1 | 01 | 95.000 VNĐ | 95.000 VNĐ | Toàn nhóm | Bộ xử lý trung tâm; giao tiếp mạng WiFi và MQTT (Broker: HiveMQ); xử lý logic các luồng I/O. |
| 2 | Động cơ Servo MG996R (Nhông kim loại, 180 độ) | 01 | 90.000 VNĐ | 90.000 VNĐ | Thành viên 2 (Trưởng nhóm) | Thiết bị Output 1; nhận lệnh từ topic `SmartLockBox_FIT24_HCMUS/dieukhien` để xoay tay đòn chốt khóa. |
| 3 | Tay đòn Servo bằng nhôm CNC (Loại 25T) | 01 | 15.000 VNĐ | 15.000 VNĐ | Thành viên 2 (Trưởng nhóm) | Thay thế tay đòn nhựa mặc định; liên kết với móc chốt nắp hộp để chịu lực kéo và chống cạy phá. |
| 4 | Module Rotary Encoder KY-040 | 01 | 10.000 VNĐ | 10.000 VNĐ | Thành viên 1 | Thiết bị Input 1; xoay núm để chọn thời gian tập trung (hiển thị trên LCD) và nhấn nút để xác nhận gửi lệnh khóa qua topic `SmartLockBox_FIT24_HCMUS/khoa`. |
| 5 | Mạch cảm biến rung/góc nghiêng SW-420 | 01 | 15.000 VNĐ | 15.000 VNĐ | Thành viên 3 | Thiết bị Input 2; phát hiện ngoại lực bất thường (lắc, cạy), truyền lệnh cảnh báo qua topic `SmartLockBox_FIT24_HCMUS/canhbao`. |
| 6 | Màn hình LCD 1602 kèm module I2C | 01 | 45.000 VNĐ | 45.000 VNĐ | Thành viên 3 | Thiết bị Output bổ sung; nhận dữ liệu từ Node-RED để hiển thị đếm ngược thời gian tập trung. |
| 7 | Mạch Còi chíp (Buzzer 5V Active) | 01 | 5.000 VNĐ | 5.000 VNĐ | Thành viên 3 | Thiết bị Output bổ sung; phát âm thanh cảnh báo/thông báo khi thời gian đếm ngược kết thúc. |
| 8 | Cuộn nhựa in 3D PLA Filament (1.75mm) | 0.5 kg | 90.000 VNĐ | 90.000 VNĐ | Toàn nhóm | Chế tạo vỏ hộp đúc phẳng, vách ngăn cách ly, cấu trúc móc khóa chữ L trên nắp. |
| 9 | Bảng cắm mạch Breadboard (Loại 400 lỗ) | 01 | 20.000 VNĐ | 20.000 VNĐ | Toàn nhóm | Bo mạch cầu nối trung gian định tuyến nguồn và dây tín hiệu giữa ESP32 và ngoại vi. |
| 10 | Dây cắm DuPont (Jumper Wires) Đực-Đực, Đực-Cái | 02 bó | 10.000 VNĐ | 20.000 VNĐ | Toàn nhóm | Dẫn dòng điện và truyền tín hiệu logic giữa các chân GPIO và các module. |
| 11 | Củ sạc điện thoại nguồn DC 5V - 2A | 01 | 40.000 VNĐ | 40.000 VNĐ | Toàn nhóm | Cấp nguồn tổng; dòng 2A đảm bảo chống sụt áp (Brownout Reset) khi Servo hoạt động. |
| 12 | Jack DC Cái & Vật tư phụ (Ốc vít M3, keo nến) | 1 bộ | 20.000 VNĐ | 20.000 VNĐ | Toàn nhóm | Jack DC chia nguồn ra Breadboard; ốc vít cố định Servo và mạch vào vỏ in 3D. |
| 13 | Nút nhấn nhả (Push Button 12x12mm) | 01 | 2.000 VNĐ | 2.000 VNĐ | Thành viên 2 (Trưởng nhóm) | Thiết bị Input phụ (YCNC ID 1); Nhấn giữ để ESP32 xóa bộ nhớ WiFi cũ và kích hoạt Access Point. |

## 2. Tổng kết Ngân sách Dự án
* **Tổng chi phí đầu tư thực tế:** ** ~462.000 VNĐ** (Bằng chữ: Khoảng bốn trăm sáu mươi hai nghìn đồng).
* **Trạng thái ngân sách:** Hợp lệ.