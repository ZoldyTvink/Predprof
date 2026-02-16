#include <VirtualWire.h>
#include <ServoTimer2.h>

#define RX_PIN 3
#define TX_PIN 11
#define SERVO_X_PIN 9
#define SERVO_Y_PIN 10
#define LASER_PIN 7

ServoTimer2 servoX;
ServoTimer2 servoY;

String device_id = "01";

enum Mode { IDLE, READY, H_SCAN, V_SCAN, GRID, DIAG_L, DIAG_R };
Mode currentMode = IDLE;

int stepIndex = -40;
unsigned long lastMoveTime = 0;
unsigned long stepDelay = 200;
bool systemStarted = false;

void sendAck(String cmd, String status) {
  String msg = "ID=" + device_id + ";SRC=A2;ACK=" + cmd + ";STATUS=" + status;
  vw_send((uint8_t*)msg.c_str(), msg.length());
  vw_wait_tx();

  Serial.print("[A2][TX] ");
  Serial.println(msg);
}

void moveTo(int x, int y) {
  int pulseX = map(x, -40, 40, 1200, 1800);
  int pulseY = map(y, -40, 40, 1200, 1800);
  servoX.write(pulseX);
  servoY.write(pulseY);
}

void startSystem() {
  systemStarted = true;
  currentMode = READY;
  servoX.attach(SERVO_X_PIN);
  servoY.attach(SERVO_Y_PIN);
  Serial.println("[A2] SYSTEM STARTED");
}

void stopSystem() {
  systemStarted = false;
  currentMode = IDLE;
  digitalWrite(LASER_PIN, LOW);
  moveTo(0, 0);
  servoX.detach();
  servoY.detach();
  Serial.println("[A2] SYSTEM STOPPED");
}

void setMode(Mode m) {
  if (!systemStarted) return;
  currentMode = m;
  stepIndex = -40;
  lastMoveTime = millis();
}

void processMode() {
  if (!systemStarted) return;
  if (currentMode == READY || currentMode == IDLE) return;

  if (millis() - lastMoveTime < stepDelay) return;
  lastMoveTime = millis();

  int x = 0, y = 0;

  if (currentMode == H_SCAN)      { x = stepIndex; y = 0; }
  else if (currentMode == V_SCAN) { x = 0; y = stepIndex; }
  else if (currentMode == GRID)   { x = stepIndex; y = stepIndex; }
  else if (currentMode == DIAG_L) { x = stepIndex; y = -stepIndex; }
  else if (currentMode == DIAG_R) { x = -stepIndex; y = -stepIndex; }

  moveTo(x, y);

  Serial.print("[A2][POS] X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.println(y);

  stepIndex += 10;
  if (stepIndex > 40) stepIndex = -40;
}

void receiveCommand() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) {
    buf[buflen] = 0;
    String msg = String((char*)buf);

    if (!msg.startsWith("ID=" + device_id)) return;

    Serial.print("[A2][RX] ");
    Serial.println(msg);

    if (msg.indexOf("CMD=START") > 0) {
      startSystem();
      sendAck("START", "OK");
    }
    else if (msg.indexOf("CMD=STOP") > 0) {
      stopSystem();
      sendAck("STOP", "OK");
    }
    else if (msg.indexOf("CMD=H_SCAN") > 0) {
      setMode(H_SCAN); sendAck("H_SCAN", "OK");
    }
    else if (msg.indexOf("CMD=V_SCAN") > 0) {
      setMode(V_SCAN); sendAck("V_SCAN", "OK");
    }
    else if (msg.indexOf("CMD=GRID") > 0) {
      setMode(GRID); sendAck("GRID", "OK");
    }
    else if (msg.indexOf("CMD=DIAG_L") > 0) {
      setMode(DIAG_L); sendAck("DIAG_L", "OK");
    }
    else if (msg.indexOf("CMD=DIAG_R") > 0) {
      setMode(DIAG_R); sendAck("DIAG_R", "OK");
    }
    else if (msg.indexOf("CMD=HOME") > 0) {
      moveTo(0, 0); sendAck("HOME", "OK");
    }
    else if (msg.indexOf("CMD=LASER_ON") > 0) {
      digitalWrite(LASER_PIN, HIGH); sendAck("LASER_ON", "OK");
    }
    else if (msg.indexOf("CMD=LASER_OFF") > 0) {
      digitalWrite(LASER_PIN, LOW); sendAck("LASER_OFF", "OK");
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);

  vw_set_rx_pin(RX_PIN);
  vw_set_tx_pin(TX_PIN);
  vw_setup(2000);
  vw_rx_start();

  moveTo(0, 0);

  Serial.println("[A2] Radio ready (RX+TX)");
}

void loop() {
  receiveCommand();
  processMode();
}
