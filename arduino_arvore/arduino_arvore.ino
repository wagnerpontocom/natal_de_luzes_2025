#define IR_SEND_PIN 9
#include <IRremote.hpp>
#include <RH_ASK.h>
#include <SPI.h>

const uint8_t RELAY_PINS[6] = {2, 3, 4, 5, 6, 7};
const uint8_t RX_PIN = 11;
const uint16_t BIT_RATE = 2000;

RH_ASK driver(BIT_RATE, RX_PIN, 12, 10, false);

bool refletor(String command) {
  String s = command;
  s.trim();
  s.toUpperCase();
  const uint16_t addr = 0xEF00;
  int cmd = -1;
  if (s == "ON") cmd = 0x03;
  else if (s == "OFF") cmd = 0x02;
  else if (s == "BRILHO+") cmd = 0x00;
  else if (s == "BRILHO-") cmd = 0x01;
  else if (s == "R") cmd = 0x04;
  else if (s == "G") cmd = 0x05;
  else if (s == "B") cmd = 0x06;
  else if (s == "W") cmd = 0x07;
  else if (s == "FLASH") cmd = 0x0B;
  else if (s == "STROBE") cmd = 0x0F;
  else if (s == "FADE") cmd = 0x13;
  else if (s == "SMOOTH") cmd = 0x17;
  else return false;
  IrSender.sendNEC(addr, (uint8_t)cmd, 0);
  return true;
}

void sendAck(const String &payload) {
  char buf[RH_ASK_MAX_MESSAGE_LEN];
  String msg = String("OK:") + payload;
  size_t n = msg.length();
  if (n >= sizeof(buf)) return;
  msg.toCharArray(buf, sizeof(buf));
  driver.send((uint8_t*)buf, strlen(buf));
  driver.waitPacketSent();
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  for (uint8_t i = 0; i < 6; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
  }
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);
  driver.init();
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);
  if (driver.recv(buf, &buflen)) {
    if (buflen >= sizeof(buf)) buflen = sizeof(buf) - 1;
    buf[buflen] = 0;
    String msg = String((char*)buf);
    if (msg.length() >= 1 && msg[0] == 'L') {
      String cmd = msg.substring(1);
      if (refletor(cmd)) {
        sendAck("L" + cmd);
      }
    } else if (msg.length() == 7 && msg[0] == 'P') {
      bool valid = true;
      for (uint8_t i = 1; i < 7; i++) {
        char ch = msg[i];
        if (ch != '0' && ch != '1') { valid = false; break; }
      }
      if (valid) {
        for (uint8_t i = 0; i < 6; i++) {
          digitalWrite(RELAY_PINS[i], msg[i + 1] == '1' ? HIGH : LOW);
        }
        sendAck(msg);
      }
    } else {
      // mensagens não reconhecidas: ignorar
    }
  }
  
  // Parser Serial desabilitado temporariamente para simplificar
}
