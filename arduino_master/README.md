# Arduino Master (Mega)

Firmware do dispositivo Master responsável por orquestrar os efeitos e enviar comandos via rádio para os Slaves.

## Hardware
- **Placa**: Arduino Mega.
- **Módulo de rádio**: FS1000A (433 MHz, ASK/OOK).

### Pinagem IR (Arduino Mega → Keyes-IR Transmitter)
- **D9** → S (pino de sinal do módulo IR)
- **5V** → VCC
- **GND** → GND
- Protocolo: NEC (38 kHz)

### Pinagem PIR (Arduino Mega → HW-416)
- **D7** → OUT (sinal do PIR)
- **5V** → VCC
- **GND** → GND
- Observação: o PIR precisa de 20–60 s após energizar para estabilizar

### Pinagem (Arduino Mega → FS1000A)
- **D12** → DATA (pino de dados do FS1000A)
- **5V** → VCC
- **GND** → GND
- **Antena**: fio ~17,3 cm (1/4 de onda) no pino ANT do módulo
- (Opcional) PTT: não utilizado; deixe desconectado

## Tabela de Pinos (Arduino Mega)

| Pino | Módulo         | Sinal/Conexão | Observações |
|------|-----------------|----------------|-------------|
| D12  | FS1000A (TX RF) | DATA           | Antena ~17,3 cm no ANT |
| 5V   | FS1000A         | VCC            | — |
| GND  | FS1000A         | GND            | — |
| D9   | Keyes-IR (TX IR) | S              | NEC 38 kHz (IRremote) |
| 5V   | Keyes-IR        | VCC            | — |
| GND  | Keyes-IR        | GND            | — |
| D7   | PIR HW-416      | OUT            | Aquecimento 20–60 s |
| 5V   | PIR HW-416      | VCC            | — |
| GND  | PIR HW-416      | GND            | — |

## Arquivos
- **arduino_master.ino**: ponto de entrada do firmware do Master.

## Build e Upload
1. Abra o projeto no Arduino IDE.
2. Selecione a placa "Arduino Mega or Mega 2560" e a porta correta.
3. Compile e faça o upload para a placa.

### Dependências
- RadioHead (RH_ASK) — para RF 433 MHz
- IRremote (by Arduino-IRremote) — para transmissão IR NEC

## Próximas etapas
- Ajustar taxa de bits no código se necessário (`BIT_RATE`, padrão 2000 bps).
- Implementar protocolo de mensagens e fila de comandos.
- Adicionar logs/serial para depuração.
