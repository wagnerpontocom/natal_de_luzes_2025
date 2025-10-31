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
bool enableLogs = false; // desabilitar logs por enquanto

int faixaMinima = 1;   // faixa mínima inclusiva para sorteio
int faixaMaxima = 5;   // faixa máxima inclusiva para sorteio
int ultimoSorteio = -1; // armazena o último número sorteado

// ------------- RF queue com ACK -------------
struct PendingRF {
  String payload;
  uint8_t retries;
  unsigned long lastSentMs;
  bool waitingAck;
};

const uint8_t RF_QUEUE_SIZE = 8;
PendingRF rfQueue[RF_QUEUE_SIZE];
uint8_t rfHead = 0, rfTail = 0;
const unsigned long RF_RETRY_MS = 300;
const uint8_t RF_MAX_RETRIES = 3;

bool enqueueRF(const String &msg) {
  uint8_t next = (rfHead + 1) % RF_QUEUE_SIZE;
  if (next == rfTail) return false; // cheio
  rfQueue[rfHead] = {msg, 0, 0, false};
  rfHead = next;
  return true;
}

bool rfSendNow(const String &message) {
  char buffer[RH_ASK_MAX_MESSAGE_LEN];
  size_t len = message.length();
  if (len >= sizeof(buffer)) return false;
  message.toCharArray(buffer, sizeof(buffer));
  bool ok = driver.send((uint8_t*)buffer, strlen(buffer));
  driver.waitPacketSent();
  return ok;
}

void pollRFAck() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);
  if (driver.recv(buf, &buflen)) {
    if (buflen >= sizeof(buf)) buflen = sizeof(buf) - 1;
    buf[buflen] = 0;
    if (enableLogs) { Serial.print("rf rx: "); Serial.println((char*)buf); }
    if (buflen >= 3 && buf[0]=='O' && buf[1]=='K' && buf[2]==':') {
      String ack = String((char*)buf + 3);
      // comparar com o item da frente da fila (quem está aguardando ACK)
      if (rfTail != rfHead) {
        PendingRF &p = rfQueue[rfTail];
        if (p.waitingAck && p.payload == ack) {
          // ACK recebido, consumir
          rfTail = (rfTail + 1) % RF_QUEUE_SIZE;
        }
      }
    }
  }
}

void processRFQueue() {
  pollRFAck();
  if (rfTail == rfHead) return; // fila vazia
  PendingRF &p = rfQueue[rfTail];
  unsigned long now = millis();
  if (!p.waitingAck) {
    if (rfSendNow(p.payload)) {
      p.waitingAck = true;
      p.lastSentMs = now;
    } else {
      // falha de envio: tentar novamente na próxima volta
    }
  } else if (now - p.lastSentMs >= RF_RETRY_MS) {
    if (p.retries >= RF_MAX_RETRIES) {
      // desistir desse item
      if (enableLogs) { Serial.print("rf drop: "); Serial.println(p.payload); }
      rfTail = (rfTail + 1) % RF_QUEUE_SIZE;
    } else {
      rfSendNow(p.payload);
      p.retries++;
      p.lastSentMs = now;
    }
  }
}

bool comandoArvore(String message) {
  return enqueueRF(message);
}

bool refletorCasa(String command) {
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

bool refletorArvore(String command) {
  comandoArvore("L"+command);
}

bool comandoCasa(String message) {
  // Espera formato: 'P' + 6 bits (0/1)
  if (message.length() != 7) return false;
  if (message[0] != 'P') return false;
  for (uint8_t i = 1; i < 7; i++) {
    char c = message[i];
    if (c != '0' && c != '1') return false;
  }
  // Aplica estado aos 6 relés no arduino_som
  for (uint8_t i = 0; i < 6; i++) {
    uint8_t idx = i + 1; // 1..6
    if (message[i + 1] == '1') {
      ligarEle(idx);
    } else {
      desligarEle(idx);
    }
  }
  return true;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  driver.init();
  IrSender.begin(IR_SEND_PIN);
  pinMode(PIR_PIN, INPUT);
  if (enableLogs) Serial.println("sistema iniciado");
  if (enableLogs) Serial.println("disparado comando a para dispositivo arvore: 11000");
  comandoArvore("11000");
  Serial1.begin(115200);
  if (modoSnifferIR) {
    IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
    if (enableLogs) { Serial.print("modo sniffer IR habilitado no pino "); Serial.println(IR_RECV_PIN); }
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
      if (enableLogs) { Serial.print("rf enviado: "); Serial.println(msg); }
      sendRFMessage(msg);
      delay(20);
    }
  }

  int pirState = digitalRead(PIR_PIN);
  if (pirState == HIGH) {
    if (lastPirState != HIGH) {
      if (enableLogs) Serial.println("movimento detectado");
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
      if (enableLogs) { Serial.print("numero sorteado: "); Serial.println(sorteado); }
      rotinaDeExecucao(sorteado);
    }
    if (enableLogs) Serial.println("disparado comando a para dispositivo arvore: 01111");
    comandoArvore("01111");
    if (enableLogs) Serial.println("refletor: ON");
    refletorCasa("ON");
    delay(100);
    if (enableLogs) Serial.println("refletor: FLASH");
    refletorCasa("FLASH");
    delay(3000);
    if (enableLogs) Serial.println("refletor: OFF");
    refletorCasa("OFF");
    if (enableLogs) Serial.println("disparado comando a para dispositivo arvore: 11000");
    comandoArvore("11000");
    delay(2000);
    loggedWaiting = false;
  }
  if (pirState == LOW && lastPirState != LOW) {
    if (enableLogs) Serial.println("aguardando movimento");
    loggedWaiting = true;
  }
  lastPirState = pirState;

  // processar RF (envio/ACK) e fila Serial1 a cada iteração
  processRFQueue();
  processSerial1Queue();
}

// processar fila de comandos para o arduino_som a cada iteração
void yield() {
  processSerial1Queue();
}

// ------------- Fila para comandos Serial1 (arduino_som) -------------
const uint8_t QUEUE_SIZE = 16;
String serial1Queue[QUEUE_SIZE];
uint8_t qHead = 0, qTail = 0;
unsigned long lastSentMs = 0;
const unsigned long QUEUE_GAP_MS = 40; // pequeno delay entre mensagens

bool enqueueSerial1(const String &line) {
  uint8_t next = (qHead + 1) % QUEUE_SIZE;
  if (next == qTail) return false; // fila cheia
  serial1Queue[qHead] = line;
  qHead = next;
  return true;
}

void processSerial1Queue() {
  if (qTail == qHead) return; // vazia
  unsigned long now = millis();
  if (now - lastSentMs < QUEUE_GAP_MS) return;
  String line = serial1Queue[qTail];
  qTail = (qTail + 1) % QUEUE_SIZE;
  Serial1.println(line);
  lastSentMs = now;
}

void somPlay(int n) {
  if (enableLogs) { Serial.print("som: PLAY "); Serial.println(n); }
  enqueueSerial1(String("PLAY ") + n);
}

void somStop() { if (enableLogs) Serial.println("som: STOP"); enqueueSerial1("STOP"); }

void somPause() { if (enableLogs) Serial.println("som: PAUSE"); enqueueSerial1("PAUSE"); }

void somNext() { if (enableLogs) Serial.println("som: NEXT"); enqueueSerial1("NEXT"); }

void somPrev() { if (enableLogs) Serial.println("som: PREV"); enqueueSerial1("PREV"); }

void somVol(int v) { if (enableLogs) { Serial.print("som: VOL "); Serial.println(v); } enqueueSerial1(String("VOL ") + v); }

void ligarEle(int idx) { if (enableLogs) { Serial.print("relay: LIGARELE "); Serial.println(idx); } enqueueSerial1(String("LIGARELE ") + idx); }

void desligarEle(int idx) { if (enableLogs) { Serial.print("relay: DESLIGARELE "); Serial.println(idx); } enqueueSerial1(String("DESLIGARELE ") + idx); }

void rotinaDeExecucao(int n) {
  Serial.print("executando rotina ");
  Serial.println(n);
}
