# Arduino Som (UNO)

Firmware responsável por tocar músicas no Shield MP3, controlar relés e receber comandos do Arduino Mega via UART dedicada.

## Hardware
- Placa: Arduino UNO
- Shield: MP3 (conforme projeto anterior; mantém a ligação existente)
- Comunicação com Master: UART cabeada (SoftwareSerial no UNO)

## Tabela de Pinos (UNO)

| Pino | Função/Dispositivo | Detalhes |
|-----:|---------------------|----------|
| D2   | RX (link Master)    | SoftwareSerial RX (do Mega Serial1 TX1=D18) |
| D4   | TX (link Master)    | SoftwareSerial TX (para Mega Serial1 RX1=D19) |
| D3   | Relé arv1A          | Saída |
| D5   | Relé arv2A          | Saída |
| D6   | Relé arv3A          | Saída |
| D7   | Relé arv1B          | Saída |
| D8   | Relé arv2B          | Saída |
| D9   | Relé arv3B          | Saída |
| A0   | Relé chuva          | Saída (digital) |
| A1   | Relé casa1          | Saída (digital) |
| A2   | Relé casa2          | Saída (digital) |
| A3   | Relé casa3          | Saída (digital) |
| A4   | Relé casa4          | Saída (digital) |
| A5   | Relé projetor       | Saída (digital) |

Observação: A ligação do Shield MP3 permanece igual ao projeto do ano passado (SPI/UART conforme seu hardware). Não alteramos pinos do shield.

## Protocolo de comandos recebidos do Master (UART 115200 bps)

Uma linha por comando, terminada por \n. Case-insensitive.

- PLAY N
- STOP
- PAUSE
- NEXT
- PREV
- VOL V
- LIGARELE X
- DESLIGARELE X
- PADRAO_ALTO
- PADRAO_BAIXO

Exemplos:
- PLAY 5\n
- VOL 25\n
- LIGARELE 1\n
- DESLIGARELE 12\n
- STOP\n

## Integração com o Master (Mega)
- Mega deve usar Serial1 (TX1=D18, RX1=D19), 115200 bps.
- Ligações: TX1 (Mega) → D2 (UNO), RX1 (Mega) ← D4 (UNO), GND comum.

## Build e Upload
1. Abra o projeto no Arduino IDE
2. Selecione Arduino UNO e porta correta
3. Compile e faça upload

## Notas
- Mantivemos o uso de `Serial` para conversar com o Shield MP3, conforme o projeto anterior.
- A comunicação com o Master foi isolada em SoftwareSerial (D2/D4) para não interferir no upload via USB.
