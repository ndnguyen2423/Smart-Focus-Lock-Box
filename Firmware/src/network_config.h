#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include <Arduino.h>
#include <PubSubClient.h> // Thư viện cài bên ngoài được set up trong platformio.ini, dùng để giao tiếp MQTT


// File.h là file khai báo lời hứa, khi được include vô main thì sẽ dán luôn nội dung trong file.h
// Bắt buộc phải có keyword là extern. Thông báo cho main.cpp biết rằng có 1 client được tạo ra ở file.cpp, hãy sử dụng chung nó.
// Nếu không có extern thì khi dán nội dung file.h vào main.cpp sẽ tạo ra thêm 1 client khác so với client trong file.cpp
// MQTT quy định chỉ có 1 client được tồn tại để tránh xung đột. Nên việc có 2 client là lỗi.
extern PubSubClient client;

// Khai báo 2 hàm phục vụ cho việc setup mạng
void setup_network();
void reconnect();


#endif