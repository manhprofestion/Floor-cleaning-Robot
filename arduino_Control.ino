#include <SoftwareSerial.h>
#include <NewPing.h>
#include <LiquidCrystal_I2C.h>


const int L1 = 2;
const int L2 = 3;
const int R1 = 4;
const int R2 = 5;
bool stringComplete = false;  // biến để kiểm tra trạng thái lệnh
const int echo_L = 6;
const int trig_L = 7;
const int echo_M = 8;
const int trig_M = 9;
const int echo_R = 10;
const int trig_R = 11;

//const int button = A2; //
//const int pump = A3; // 2 chân này đang phân vân
int motor_speed = 255;    //min =125, max=255
int max_distance = 200;
int distance_L = 0;
int distance_M = 0;
int distance_R = 0;
char incomingByte;
//Tính toán đo khoảng cách
NewPing sonar_L(trig_L, echo_L, max_distance);
NewPing sonar_M(trig_M, echo_M, max_distance);
NewPing sonar_R(trig_R, echo_R, max_distance);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Tx gửi, Rx nhận (Giao tiếp UART)
const byte Rx = 12;
const byte Tx = 13;
SoftwareSerial mySerial = SoftwareSerial(Rx, Tx);
String data = "";
String dataEnd = "";


void setup()
{

  mySerial.begin(9600);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  //  pinMode(button, INPUT);
  //  pinMode(pump, OUTPUT);
  //  digitalWrite(pump, LOW);
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);

  lcd.begin();
  lcd.setCursor(0, 4); // hàng 0, cột 4
  lcd.print("Welcome!");
  Serial.begin(9600);
  delay(500);
}

// main
void loop()
{
  dataEnd = CheckData_Esp();

if (dataEnd.indexOf("Control") >= 0) // Nếu chế độ điều khiển được kích hoạt
  {
    lcd.clear(); // Xóa màn hình LCD
    lcd.print("Manual Mode"); // Hiển thị chế độ điều khiển bằng LCD
    while (true)
    {
      manualMode(); // Gọi hàm điều khiển chế độ manual

      if (dataEnd.indexOf("Auto") >= 0) // Nếu chế độ tự động được kích hoạt
      {
        moveStop(); // Dừng động cơ
        break; // Thoát khỏi vòng lặp
      }
    }
  }

if (dataEnd.indexOf("Auto") >= 0) // Nếu chế độ tự động được kích hoạt
  {
    lcd.clear(); // Xóa màn hình LCD
    lcd.print("Automatic Mode"); // Hiển thị chế độ tự động bằng LCD
      automaticMode(); // Gọi hàm điều khiển chế độ tự động

//  if (dataEnd.indexOf("Control") >= 0) // Nếu chế độ điều khiển được kích hoạt
//      {
//        moveStop(); // Dừng động cơ
//        manualMode(); // Chuyển sang chế độ điều khiển manual
//        break;
//      }
  }
}


 
  // Đọc giá trị từ Esp gửi tới Uno
  String CheckData_Esp (void)
  {
    while (!mySerial.available()) delay(1);
    String data = mySerial.readStringUntil('\n');
    Serial.print("Dữ liệu nhận được từ ESP: ");
    Serial.println(data);
    return data;
  }


  void manualMode()
  {
    //  lcd.clear();
    //  lcd.print("ManuaMode");

    if (mySerial.available() > 0)   //Kiểm tra xem Nano có dữ liệu từ esp gửi về k
    {
      dataEnd = CheckData_Esp();
      stringComplete = true;


      if (dataEnd.indexOf("F") >= 0)
      {
        moveForward();
        lcd.clear();
        lcd.print("Forward");
        Serial.println("Đi thẳng");
      }

      else if (dataEnd.indexOf("B") >= 0)
      {
        moveBackward();
        lcd.clear();
        lcd.print("Backward");
        Serial.println("Đi lùi");
      }

      else if (dataEnd.indexOf("L") >= 0)
      {
        moveLeft();
        lcd.clear();
        lcd.print("Left");
        Serial.println("Rẽ trái");
      }

      else if (dataEnd.indexOf("R") >= 0)
      {
        moveRight();
        lcd.clear();
        lcd.print("Right");
        Serial.println("Rẽ phải");
      }

      else if (dataEnd.indexOf("S") >= 0)
      {
        moveStop();
        lcd.clear();
        lcd.print("Stop");
        Serial.println("Dừng lại");
      }

      else if (dataEnd.indexOf("1") >= 0)
      {
        motor_speed = 155;
        Serial.println("Đang đi tốc độ 1");
      }

      else if (dataEnd.indexOf("2") >= 0)
      {
        motor_speed = 205;
        Serial.println("Đang đi tốc độ 2");
      }

      else if (dataEnd.indexOf("3") >= 0)
      {
        motor_speed = 255;
        Serial.println("Đang đi tốc độ 3");
      }
    }
    else
    {
      data = "";
      dataEnd = "";
      stringComplete = false;
    }

  }

  // Hàm tự động lau sàn
  void automaticMode()
  {
    dataEnd = CheckData_Esp();
  while(dataEnd)
  {
    //  Serial.println("Đã đọc đến hàm automaticMode");
    distance_L = readSensor_L();    //đọc giá trị khoảng cách và gán vào distance
    distance_M = readSensor_M();
    distance_R = readSensor_R();

    //Hiển thị khoảng cách lên LCD
    lcd.clear();
    lcd.print("L=");
    lcd.print(distance_L);
    lcd.print("cm ");
    lcd.print("M=");
    lcd.print(distance_M);
    lcd.print("cm");
    lcd.setCursor(0, 1);
    lcd.print("R=");
    lcd.print(distance_R);
    lcd.print("cm");

    if (distance_M <= 20)   //nếu khoảng cách ở giữa nhỏ hơn 20
    {
      if (distance_R > distance_L)  //Kiểm tra bên phải và bên trái
      {
        if ((distance_R <= 20) && (distance_L <= 20))   //nếu khoảng cách ở 2 bên quá gần vật cản thì dừng
        {
          moveStop();
          delay(200);
          moveBackward();   //quay lại
          delay(2000);
        }
        else   // ngược lại nếu nếu khoảng cách trái,phải đều xa hơn hơn 20 và bên phải xa hơn bên trái ở giữa thì gần
        {
          moveBackward();  //quay lại rồi rẽ phải
          delay(500);
          moveRight();
          delay(2000);
        }
      }
      else if (distance_R < distance_L)
      {
        if ((distance_R <= 20) && (distance_L <= 20)) // Nếu bên trải lớn hơn bên phải và khoảng cách 2 bên đều nhỏ hơn 20 thì đi lùi
        {
          moveStop();   //move back
          delay(200);
          moveBackward();
          delay(2000);
        }
        else // ngược lại nếu 2 bên trái phải đều lơn hơn 20 thì sẽ tiếng lên khoảng 1 đoạn nhỏ nữa rồi rẽ trái
        {
          moveBackward();   //move back then turn left
          delay(500);
          moveLeft();
          delay(2000);
        }
      }
    }

    else if (distance_R <= 15)  //Nếu cảm biến phải nhỏ hơn 15cm
    {
      moveLeft();   //rẽ trái
      delay(500);
    }
    else if (distance_L <= 15)  //Nếu cảm biến trái nhỏ hơn 15cm
    {
      moveRight();    //turn phải
      delay(500);
    }
    else
    {
      moveForward();   // ngược lại với các điều kiện kia thì sẽ đi thẳng
    }
    ////////////////////
    
    if (dataEnd.indexOf("Control") >= 0) // Nếu chế độ điều khiển được kích hoạt
      {
         manualMode();
      }
  }
  }
  int readSensor_L()    //Đọc khoảng cách cảm biển bên trái
  {
    delay(70);
    int cm_L = sonar_L.ping_cm();
    if (cm_L == 0)
    {
      cm_L = 250;
    }
    return cm_L;
  }

  int readSensor_M()    //Đọc khoảng cách cảm biển giữa
  {
    delay(70);
    int cm_M = sonar_M.ping_cm();
    if (cm_M == 0)
    {
      cm_M = 250;
    }
    return cm_M;
  }

  int readSensor_R()   //Đọc khoảng cách cảm biển bên phải
  {
    delay(70);
    int cm_R = sonar_R.ping_cm();
    if (cm_R == 0)
    {
      cm_R = 250;
    }
    return cm_R;
  }



  void moveForward()
  {
    digitalWrite(L1, LOW);
    analogWrite(L2, motor_speed);
    analogWrite(R1, motor_speed);
    digitalWrite(R2, LOW);
  }

  void moveBackward()
  {
    analogWrite(L1, motor_speed);
    digitalWrite(L2, LOW);
    digitalWrite(R1, LOW);
    analogWrite(R2, motor_speed);
  }

  void moveRight()
  {
    analogWrite(L1, motor_speed);
    digitalWrite(L2, LOW);
    analogWrite(R1, motor_speed);
    digitalWrite(R2, LOW);
  }

  void moveLeft()
  {
    digitalWrite(L1, LOW);
    analogWrite(L2, motor_speed);
    digitalWrite(R1, LOW);
    analogWrite(R2, motor_speed);
  }

  void moveStop()
  {
    digitalWrite(L1, LOW);
    digitalWrite(L2, LOW);
    digitalWrite(R1, LOW);
    digitalWrite(R2, LOW);
  }
