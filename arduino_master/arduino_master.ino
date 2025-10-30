#include <RH_ASK.h>
#include <SPI.h>
#include <IRremote.h>

const uint8_t TX_PIN = 12;
const uint16_t BIT_RATE = 2000;
const uint8_t IR_SEND_PIN = 9;
const uint8_t PIR_PIN = 7;

RH_ASK driver(BIT_RATE, 11, TX_PIN, 10, false);


bool enviaArvore(String message) {
  return sendRFMessage(message);
}

bool sendRFMessage(String message) {
  char buffer[RH_ASK_MAX_MESSAGE_LEN];
  size_t len = message.length();
  if (len >= sizeof(buffer)) return false;
  message.toCharArray(buffer, sizeof(buffer));
  bool ok = driver.send((uint8_t*)buffer, strlen(buffer));
  driver.waitPacketSent();
  return ok;
}

bool refletor(String command) {
  String s = command;
  s.trim();
  s.toUpperCase();
  uint32_t code = 0;
  if (s == "ON") code = 0x00FF02FD;
  else if (s == "OFF") code = 0x00FF827D;
  else if (s == "BRILHO+") code = 0x00FF3AC5;
  else if (s == "BRILHO-") code = 0x00FFBA45;
  else if (s == "R") code = 0x00FF1AE5;
  else if (s == "G") code = 0x00FF9A65;
  else if (s == "B") code = 0x00FFA25D;
  else if (s == "W") code = 0x00FF22DD;
  else if (s == "FLASH") code = 0x00FFB24D;
  else if (s == "STROBE") code = 0x00FF32CD;
  else if (s == "FADE") code = 0x00FF20DF;
  else if (s == "SMOOTH") code = 0x00FFA05F;
  else return false;
  IrSender.sendNEC(code, 32);
  return true;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  driver.init();
  IrSender.begin(IR_SEND_PIN);
  pinMode(PIR_PIN, INPUT);
  enviaArvore("11000");
  Serial1.begin(115200);
}

void loop() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      sendRFMessage(msg);
      delay(20);
    }
  }

  int pirState = digitalRead(PIR_PIN);
  if (pirState == HIGH) {
    enviaArvore("01111");
    refletor("ON");
    delay(100);
    refletor("FLASH");
    delay(3000);
    refletor("OFF");
    enviaArvore("11000");
    delay(2000);
  }
}

void somPlay(int n) {
  Serial1.print("PLAY ");
  Serial1.println(n);
}

void somStop() {
  Serial1.println("STOP");
}

void somPause() {
  Serial1.println("PAUSE");
}

void somNext() {
  Serial1.println("NEXT");
}

void somPrev() {
  Serial1.println("PREV");
}

void somVol(int v) {
  Serial1.print("VOL ");
  Serial1.println(v);
}

void ligarEle(int idx) {
  Serial1.print("LIGARELE ");
  Serial1.println(idx);
}

void desligarEle(int idx) {
  Serial1.print("DESLIGARELE ");
  Serial1.println(idx);
}
