#include <SoftwareSerial.h>

unsigned char cmd_buf[10];
unsigned char i;
int randomNumber = 0;
int ultimoSorteado = 0;

const int arv1A = 3;
const int arv2A = 5;
const int arv3A = 6;
const int arv1B = 7;
const int arv2B = 8;
const int arv3B = 9;

SoftwareSerial linkMaster(10, 4); // RX=D10, TX=D4

void ArduinoMP3Shield_SendCMD(unsigned char *cmd_buf, unsigned len) {
  unsigned i;
  for (i = 0; i < len; i++) {
    Serial.write(cmd_buf[i]);
  }
}

void setVolume(int volume) {
  if (volume < 0) volume = 0;
  if (volume > 31) volume = 31;
  cmd_buf[0] = 0x7E;
  cmd_buf[1] = 0x03;
  cmd_buf[2] = 0xA7;
  cmd_buf[3] = volume;
  cmd_buf[4] = 0x7E;
  ArduinoMP3Shield_SendCMD(cmd_buf, 5);
}

void play(int track) {
  unsigned char trackHigh = (track >> 8) & 0xFF;
  unsigned char trackLow = track & 0xFF;
  cmd_buf[0] = 0x7E;
  cmd_buf[1] = 0x04;
  cmd_buf[2] = 0xA0;
  cmd_buf[3] = trackHigh;
  cmd_buf[4] = trackLow;
  cmd_buf[5] = 0x7E;
  ArduinoMP3Shield_SendCMD(cmd_buf, 6);
  delay(100);
}

void togglePlayPause() {
  cmd_buf[0] = 0x7E;
  cmd_buf[1] = 0x02;
  cmd_buf[2] = 0xA3;
  cmd_buf[3] = 0x7E;
  ArduinoMP3Shield_SendCMD(cmd_buf, 4);
}

void stop() {
  cmd_buf[0] = 0x7E;
  cmd_buf[1] = 0x02;
  cmd_buf[2] = 0xA4;
  cmd_buf[3] = 0x7E;
  ArduinoMP3Shield_SendCMD(cmd_buf, 4);
}

void nextMusic() {
  cmd_buf[0] = 0x7E;
  cmd_buf[1] = 0x02;
  cmd_buf[2] = 0xA5;
  cmd_buf[3] = 0x7E;
  ArduinoMP3Shield_SendCMD(cmd_buf, 4);
}

void previousMusic() {
  cmd_buf[0] = 0x7E;
  cmd_buf[1] = 0x02;
  cmd_buf[2] = 0xA6;
  cmd_buf[3] = 0x7E;
  ArduinoMP3Shield_SendCMD(cmd_buf, 4);
}

void randomMusic() {
  cmd_buf[0] = 0x7E;
  cmd_buf[1] = 0x03;
  cmd_buf[2] = 0xA9;
  cmd_buf[3] = 0x03;
  cmd_buf[4] = 0x7E;
  ArduinoMP3Shield_SendCMD(cmd_buf, 5);
}

void log(String texto){
  Serial.println(texto);
  delay(50);
}

void log_(String texto){
  Serial.print(texto);
}

void onArv1A(){ digitalWrite(arv1A, HIGH); }
void offArv1A(){ digitalWrite(arv1A, LOW); }
void onArv2A(){ digitalWrite(arv2A, HIGH); }
void offArv2A(){ digitalWrite(arv2A, LOW); }
void onArv3A(){ digitalWrite(arv3A, HIGH); }
void offArv3A(){ digitalWrite(arv3A, LOW); }
void onArv1B(){ digitalWrite(arv1B, LOW); }
void offArv1B(){ digitalWrite(arv1B, HIGH); }
void onArv2B(){ digitalWrite(arv2B, LOW); }
void offArv2B(){ digitalWrite(arv2B, HIGH); }
void onArv3B(){ digitalWrite(arv3B, LOW); }
void offArv3B(){ digitalWrite(arv3B, HIGH); }

void padraoBaixo(){
  onArv1B(); onArv2B(); onArv3B();
  offArv1A(); offArv2A(); offArv3A();
}

void padraoAlto(){
  offArv1B(); offArv2B(); offArv3B();
  onArv1A(); onArv2A(); onArv3A();
}

bool setRelay(uint8_t idx, bool on){
  switch(idx){
    case 1: on? onArv1A(): offArv1A(); break;
    case 2: on? onArv2A(): offArv2A(); break;
    case 3: on? onArv3A(): offArv3A(); break;
    case 4: on? onArv1B(): offArv1B(); break;
    case 5: on? onArv2B(): offArv2B(); break;
    case 6: on? onArv3B(): offArv3B(); break;
    default: return false;
  }
  return true;
}

void setup(){
  Serial.begin(9600);
  linkMaster.begin(115200);

  pinMode(arv1A, OUTPUT);
  pinMode(arv2A, OUTPUT);
  pinMode(arv3A, OUTPUT);
  pinMode(arv1B, OUTPUT);
  pinMode(arv2B, OUTPUT);
  pinMode(arv3B, OUTPUT);

  setVolume(25);
  delay(100);
  stop();
  delay(200);
  padraoAlto();
}

void loop(){
  if (linkMaster.available()){
    String line = linkMaster.readStringUntil('\n');
    line.trim();
    if (line.length()==0) return;

    String u = line;
    u.toUpperCase();

    if (u.length() == 7 && u[0] == 'P') {
      bool valid = true;
      for (uint8_t i = 1; i < 7; i++) { char c = u[i]; if (c!='0' && c!='1') { valid=false; break; } }
      if (valid) {
        for (uint8_t i = 0; i < 6; i++) {
          bool on = (u[i+1] == '1');
          setRelay(i+1, on);
        }
        linkMaster.print("OK:");
        linkMaster.println(u);
      }
    } else if (u.startsWith("PLAY ")){
      int n = u.substring(5).toInt();
      if (n>0){ play(n); }
    } else if (u == "STOP"){
      stop();
    } else if (u == "PAUSE"){
      togglePlayPause();
    } else if (u == "NEXT"){
      nextMusic();
    } else if (u == "PREV"){
      previousMusic();
    } else if (u.startsWith("VOL ")){
      int v = u.substring(4).toInt();
      setVolume(v);
    } else if (u.startsWith("LIGARELE ")){
      int idx = u.substring(9).toInt();
      setRelay(idx, true);
    } else if (u.startsWith("DESLIGARELE ")){
      int idx = u.substring(11).toInt();
      setRelay(idx, false);
    } else if (u == "PADRAO_ALTO"){
      padraoAlto();
    } else if (u == "PADRAO_BAIXO"){
      padraoBaixo();
    }
  }
}
