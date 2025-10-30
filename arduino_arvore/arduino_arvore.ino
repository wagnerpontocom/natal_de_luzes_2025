#include <RH_ASK.h>
#include <SPI.h>

const uint8_t RX_PIN = 11;
const uint16_t BIT_RATE = 2000;

RH_ASK driver(BIT_RATE, RX_PIN, 12, 10, false);

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
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
}
