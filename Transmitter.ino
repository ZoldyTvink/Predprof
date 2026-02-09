#include <VirtualWire.h>

const int transmit_pin = 3;
const int laser_pin = 7;

void setup() {
  Serial.begin(115200);
  
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);
  
  pinMode(laser_pin, OUTPUT);
  digitalWrite(laser_pin, LOW);

  Serial.println("Передатчик готов! Введите команду:");
  Serial.println("LASER 1 - Включить лазер");
  Serial.println("LASER 0 - Выключить лазер");
  Serial.println("RUN H - Горизонтальное сканирование");
  Serial.println("RUN V - Вертикальное сканирование");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "LASER 1") {
      sendCommand("LASER 1");
      Serial.println("Команда отправлена: Включить лазер");
    }
    else if (command == "LASER 0") {
      sendCommand("LASER 0");
      Serial.println("Команда отправлена: Выключить лазер");
    }
    else if (command == "RUN H") {
      sendCommand("RUN H");
      Serial.println("Команда отправлена: Горизонтальное сканирование");
    }
    else if (command == "RUN V") {
      sendCommand("RUN V");
      Serial.println("Команда отправлена: Вертикальное сканирование");
    }
    else {
      Serial.println("Неизвестная команда, попробуйте снова.");
    }
  }
}

void sendCommand(const char *command) {
  vw_send((uint8_t *)command, strlen(command));
  vw_wait_tx();
  Serial.print("Отправлено: ");
  Serial.println(command);
}
