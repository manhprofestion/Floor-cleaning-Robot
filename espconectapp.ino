//#define ENA   5          // Điều khiển tốc độ động cơ bên trái     GPIO5(D1)
//#define ENB   12          // Điều khiển tốc độ động cơ bên phải    GPIO12(D6)
//#define IN1  4          // L298N in1 Động cơ trái quay             GPIO4(D2)
//#define IN2  0          // L298N in2 Động cơ trái quay ngược lại   GPIO0(D3)
//#define IN3  2           // L298N in3 Động cơ phải quay            GPIO2(D4)
//#define IN4  14           // L298N in4 Động cơ phải quay ngược lại GPIO14(D5)
// L298N in1 Động cơ trái quay             GPIO4(D2)


#include <ESP8266WiFi.h>
WiFiClient client;
WiFiServer server(80);


int pump = 4; //D2
// #define  pump 4


/* WIFI settings */
const char* ssid = "ipxs";
const char* password = "11111111";
String  SendUno = "";
String request = "";
String data = "";
String dataPump = ""; // khai báo dữ liệu nhận để bơm nước

#include <SoftwareSerial.h>
const byte TX = D5;//12
const byte RX = D6;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);

void setup()
{

 
  pinMode(pump, OUTPUT);
  digitalWrite(pump, HIGH);
  // Serial.println(pump);

  Serial.begin(115200);
  mySerial.begin(9600);
  connectWiFi();
  server.begin();
}


void connectWiFi()
{
  Serial.println("Đang kết nối tới Wifi: ");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(300);
    Serial.print("....");
  }
  Serial.println("");
  Serial.println("Đã kết nối");
  Serial.println("Địa chỉ IP Esp8266 : ");
  Serial.print((WiFi.localIP()));
}
void loop()
{

  // kiểm tra xem app inventor có được kết nối với esp k
  client = server.available();
  if (!client) return;
  data = checkClient ();
  Send_data();
  //ControlPump();
//  automatic();
}

void Send_data()
{
  // data = checkClient ();
  if (data == "Off" || data == "On")
  {
   
    
        if (data == "On")
        {
          digitalWrite(pump, LOW);
          //    mySerial.write('P'); // Gửi lệnh sang Arduino để hiển thị lên hiển thị LCD
          //    delay(100);
          
          Serial.println("Đang bơm nước");
        }

        else if (data  == "Off")
        {
          digitalWrite(pump, HIGH);
          //    mySerial.write('T'); // Gửi lệnh sang Arduino để hiển thị lên hiển thị LCD
          //    delay(100);
          Serial.println("Máy bơm đang được tắt");
          
        }

    
  }
  else if(data != "On" && data != "Off")
  {
    SendUno = "";
    SendUno = String(data);
    Serial.println("Dữ liệu được gửi tới Uno: ");
    Serial.println(SendUno);
    mySerial.println(SendUno);
  }

}
String checkClient (void) // Đọc giá trị gửi đến từ app Inventor gửi về
{
  while (!client.available()) delay(1);
  String request = client.readStringUntil('\r');
  Serial.println(request);
  request.remove(0, 5);
  Serial.println(request);
  request.remove(request.length() - 9, 9);
  Serial.println(request);
  return request;

}



// Xử lý 2 chân Button và Pump

// void ControlPump()
// {
//   if (dataPump.indexOf("On") >= 0)
//   {
//     digitalWrite(pump, HIGH);
//     //    mySerial.write('P'); // Gửi lệnh sang Arduino để hiển thị lên hiển thị LCD
//     //    delay(100);
//     Serial.println("Đang bơm nước");
//   }

//   else if (dataPump.indexOf("Off") >= 0)
//   {
//     digitalWrite(pump, LOW);
//     //    mySerial.write('T'); // Gửi lệnh sang Arduino để hiển thị lên hiển thị LCD
//     //    delay(100);
//     Serial.println("Máy bơm đang được tắt");
//   }
// }


//// 1 đoạn chương trình sử dụng nút nhấn để bật or tắt Robot tự động lau sàn
//void HandleButton()
//{
//
//  
//  while( (WiFi.status() != WL_CONNECTED)  ))// Kiểm tra xem có kết nối wifi không
//  {
//    Serial.println("Đã mất kết nối wifi");
//    digitalWrite(ledPin, HIGH);  // Bật đèn LED
//    Serial.println("Led đang sáng");
//    delay(1000);
//    digitalWrite(ledPin, LOW);  // Tắt đèn LED
//    Serial.println("Led đang tắt");
//    delay(1000);
//  }
//}
