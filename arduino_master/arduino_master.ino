#include <RH_ASK.h>
#include <SPI.h>
#include <IRremote.h>

const uint8_t TX_PIN = 12;
const uint16_t BIT_RATE = 2000;
const uint8_t IR_SEND_PIN = 9;
const uint8_t PIR_PIN = 7;
const uint8_t IR_RECV_PIN = 2;

RH_ASK driver(BIT_RATE, 11, TX_PIN, 10, false);

bool loggedWaiting = false;
int lastPirState = LOW;
bool modoSnifferIR = false; // false = comportamento atual; true = ler IR e logar no console

int faixaMinima = 1;   // faixa mínima inclusiva para sorteio
int faixaMaxima = 5;   // faixa máxima inclusiva para sorteio
int ultimoSorteio = -1; // armazena o último número sorteado


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
  Serial.print("IR enviar -> addr=");
  Serial.print(addr, HEX);
  Serial.print(" cmd=");
  Serial.println(cmd, HEX);
  IrSender.sendNEC(addr, (uint8_t)cmd, 0);
  return true;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  driver.init();
  IrSender.begin(IR_SEND_PIN);
  pinMode(PIR_PIN, INPUT);
  Serial.println("sistema iniciado");
  Serial.println("disparado comando a para dispositivo arvore: 11000");
  enviaArvore("11000");
  Serial1.begin(115200);
  if (modoSnifferIR) {
    IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
    Serial.print("modo sniffer IR habilitado no pino ");
    Serial.println(IR_RECV_PIN);
  }
  randomSeed(analogRead(A0) ^ micros());
}

void loop() {
  if (modoSnifferIR) {
    if (IrReceiver.decode()) {
      Serial.print("IR recebido: protocolo=");
      Serial.print(IrReceiver.decodedIRData.protocol);
      Serial.print(" addr=");
      Serial.print(IrReceiver.decodedIRData.address, HEX);
      Serial.print(" cmd=");
      Serial.print(IrReceiver.decodedIRData.command, HEX);
      Serial.print(" raw=");
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.resume();
    }
    return; // não executa o restante quando em modo sniffer
  }

  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      Serial.print("rf enviado: ");
      Serial.println(msg);
      sendRFMessage(msg);
      delay(20);
    }
  }

  int pirState = digitalRead(PIR_PIN);
  if (pirState == HIGH) {
    if (lastPirState != HIGH) {
      Serial.println("movimento detectado");
      // sorteio inclusivo na faixa [faixaMinima, faixaMaxima] e diferente do último
      int sorteado;
      int minV = faixaMinima;
      int maxV = faixaMaxima;
      if (minV > maxV) { int tmp = minV; minV = maxV; maxV = tmp; }
      if (minV == maxV) {
        sorteado = minV;
      } else {
        do {
          sorteado = random(minV, maxV + 1);
        } while (sorteado == ultimoSorteio);
      }
      ultimoSorteio = sorteado;
      Serial.print("numero sorteado: ");
      Serial.println(sorteado);
      rotinaDeExecucao(sorteado);
    }
    Serial.println("disparado comando a para dispositivo arvore: 01111");
    enviaArvore("01111");
    Serial.println("refletor: ON");
    refletor("ON");
    delay(100);
    Serial.println("refletor: FLASH");
    refletor("FLASH");
    delay(3000);
    Serial.println("refletor: OFF");
    refletor("OFF");
    Serial.println("disparado comando a para dispositivo arvore: 11000");
    enviaArvore("11000");
    delay(2000);
    loggedWaiting = false;
  }
  if (pirState == LOW && lastPirState != LOW) {
    Serial.println("aguardando movimento");
    loggedWaiting = true;
  }
  lastPirState = pirState;
}

void somPlay(int n) {
  Serial.print("som: PLAY ");
  Serial.println(n);
  Serial1.print("PLAY ");
  Serial1.println(n);
}

void somStop() {
  Serial.println("som: STOP");
  Serial1.println("STOP");
}

void somPause() {
  Serial.println("som: PAUSE");
  Serial1.println("PAUSE");
}

void somNext() {
  Serial.println("som: NEXT");
  Serial1.println("NEXT");
}

void somPrev() {
  Serial.println("som: PREV");
  Serial1.println("PREV");
}

void somVol(int v) {
  Serial.print("som: VOL ");
  Serial.println(v);
  Serial1.print("VOL ");
  Serial1.println(v);
}

void ligarEle(int idx) {
  Serial.print("relay: LIGARELE ");
  Serial.println(idx);
  Serial1.print("LIGARELE ");
  Serial1.println(idx);
}

void desligarEle(int idx) {
  Serial.print("relay: DESLIGARELE ");
  Serial.println(idx);
  Serial1.print("DESLIGARELE ");
  Serial1.println(idx);
}

void rotinaDeExecucao(int n) {
  Serial.print("executando rotina ");
  Serial.println(n);
}
