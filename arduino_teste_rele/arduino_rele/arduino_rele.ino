
const uint8_t porta[6] = {2, 3, 4, 5, 6, 7};

void setup() {
  pinMode(porta[0], OUTPUT);
  pinMode(porta[1], OUTPUT);

  Serial.begin(9600);
  Serial.println("Iniciando...");
}

void loop() {
  
  Serial.println("Ligando sequencia...");
  for (int i = 0; i <= 2; i++){
    digitalWrite(porta[i], HIGH);
    delay(300);
  }

  delay(2000);

  Serial.println("Desligando sequencia...");
  for (int i = 0; i <= 2; i++){
    digitalWrite(porta[i], LOW);
    delay(300);
  }
  
  
  delay(2000);
}
