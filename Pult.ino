#include <VirtualWire.h>

#define TX_PIN 11
#define RX_PIN 2

String device_id = "01";
unsigned long lastSend = 0;

void setup() {
  Serial.begin(9600);
  vw_set_tx_pin(TX_PIN);
  vw_set_rx_pin(RX_PIN);
  vw_setup(2000);
  vw_rx_start();

  Serial.println("[A1] Radio ready (TX+RX)");
  Serial.println("Commands: START, H_SCAN, V_SCAN, GRID, DIAG_L, DIAG_R, HOME, LASER_ON, LASER_OFF, STOP");
}

void sendCommand(String cmd) {
  String msg = "ID=" + device_id + ";SRC=A1;CMD=" + cmd;
  vw_send((uint8_t*)msg.c_str(), msg.length());
  vw_wait_tx();

  Serial.print("[A1][TX] ");
  Serial.println(msg);
}

void receiveAck() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) {
    buf[buflen] = 0;
    String msg = String((char*)buf);

    if (!msg.startsWith("ID=" + device_id)) return;

    Serial.print("[A1][RX] ");
    Serial.println(msg);
  }
}

void loop() {
  receiveAck();

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd.length() > 0) {
      sendCommand(cmd);
    }
  }
}
