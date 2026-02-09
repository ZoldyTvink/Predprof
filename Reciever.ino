#include <VirtualWire.h>
#include <ServoTimer2.h>  
const int receive_pin = 2;   
const int laser_pin = 7;     
const int servo_pin_1 = 9;   
const int servo_pin_2 = 10;  

ServoTimer2 servo1;
ServoTimer2 servo2;

char received_message[50];

void setup() {
  Serial.begin(115200);

  
  vw_set_rx_pin(receive_pin);
  vw_setup(2000);  
  vw_rx_start();  
  Serial.println("Приемник готов!");
  
  
  servo1.attach(servo_pin_1);
  servo2.attach(servo_pin_2);
  
 
  pinMode(laser_pin, OUTPUT);
  digitalWrite(laser_pin, LOW);  
  
  Serial.println("Приемник готов и слушает команды!");
}

void loop() {
  uint8_t message_length = 50;
  
  
  if (vw_get_message((uint8_t *)received_message, &message_length)) {
    received_message[message_length] = '\0';  
    Serial.print("Получено: ");
    Serial.println(received_message);

    
    if (strcmp(received_message, "LASER 1") == 0) {
      digitalWrite(laser_pin, HIGH);  
      Serial.println("Лазер включен");
    } else if (strcmp(received_message, "LASER 0") == 0) {
      digitalWrite(laser_pin, LOW); 
      Serial.println("Лазер выключен");
    } else if (strcmp(received_message, "RUN H") == 0) {
      scanHorizontal();  
    } else if (strcmp(received_message, "RUN V") == 0) {
      scanVertical(); 
    }
  }
}

void scanHorizontal() {
  Serial.println("Начинаю горизонтальное сканирование...");
  for (int angle = -40; angle <= 40; angle += 10) {
    servo1.write(90 + angle);  
    servo2.write(90);  
    delay(3000);  
  }
  servo1.write(90); 
  servo2.write(90);
  Serial.println("Горизонтальное сканирование завершено");
}

void scanVertical() {
  Serial.println("Начинаю вертикальное сканирование...");
  for (int angle = -40; angle <= 40; angle += 10) {
    servo1.write(90);  
    servo2.write(90 + angle);  
    delay(3000);  
  }
  servo1.write(90);  
  servo2.write(90);
  Serial.println("Вертикальное сканирование завершено");
}
