#include <RH_ASK.h>
#include <SPI.h>

const uint8_t RELAY_PINS[6] = {2, 3, 4, 5, 6, 7};
const uint8_t RX_PIN = 11;
const uint16_t BIT_RATE = 2000;

RH_ASK driver(BIT_RATE, RX_PIN, 12, 10, false);

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  for (uint8_t i = 0; i < 6; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
  }
  driver.init();
  Serial.println("RH_ASK receptor iniciado");
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);
  if (driver.recv(buf, &buflen)) {
    if (buflen >= sizeof(buf)) buflen = sizeof(buf) - 1;
    buf[buflen] = 0;
    if (strcmp((char*)buf, "11000") == 0) {
      Serial.println("árvore low");
    } else if (strcmp((char*)buf, "01111") == 0) {
      Serial.println("árvore high");
    } else {
      Serial.print("Recebido: ");
      Serial.println((char*)buf);
    }
  }
  
  static char serialBuf[8];
  static uint8_t idx = 0;
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (idx == 0) {
      if (c == 'P') {
        serialBuf[idx++] = c;
      } else {
        // ignore until we see a 'P'
      }
    } else {
      serialBuf[idx++] = c;
      if (idx >= 7) {
        // We have 7 chars: P + 6 bits
        bool valid = true;
        for (uint8_t i = 1; i < 7; i++) {
          if (serialBuf[i] != '0' && serialBuf[i] != '1') {
            valid = false;
            break;
          }
        }
        if (valid && serialBuf[0] == 'P') {
          for (uint8_t i = 0; i < 6; i++) {
            digitalWrite(RELAY_PINS[i], serialBuf[i + 1] == '1' ? HIGH : LOW);
          }
        }
        idx = 0; // reset buffer for next message
      }
    }
  }
}
