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

### Botão/Chave do Modo CLI (sem resistores externos)
- **A1 (MODE_PIN)** → Entrada com `INPUT_PULLUP`.
- **A2 (MODE_GND_PIN)** → Saída com nível LOW.
- **Ligação**: conecte um botão/chave entre A1 e A2.
  - Chave ABERTA: A1 lê HIGH → Modo normal (PIR/rotinas ativas).
  - Chave FECHADA: A1 lê LOW → Modo CLI via Serial ativado.
  - Não conecte A2 diretamente ao GND externo (ele já fica em LOW via software).

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
| A1   | Modo CLI        | MODE_PIN       | Entrada `INPUT_PULLUP` |
| A2   | Modo CLI        | MODE_GND_PIN   | Saída fixa em LOW |

## Arquivos
- **arduino_master.ino**: ponto de entrada do firmware do Master.

## Build e Upload
1. Abra o projeto no Arduino IDE.
2. Selecione a placa "Arduino Mega or Mega 2560" e a porta correta.
3. Compile e faça o upload para a placa.

### Dependências
- RadioHead (RH_ASK) — para RF 433 MHz
- IRremote (by Arduino-IRremote) — para transmissão IR NEC

## Modo Serial CLI (quando a chave A1–A2 estiver FECHADA)
Taxa: 9600 bps. Envie uma linha por comando (terminada em Enter):

- **Árvore via RF**
  - `ARVORE P110000` — Envia padrão P para o arduino_arvore
  - `ARVORE LON` / `ARVORE LOFF` — Payload direto RF
  - Qualquer linha não reconhecida cai em `comandoArvore(line)`

- **Árvore via IR**
  - `ARVORE IR ON|OFF|FLASH|STROBE|FADE|SMOOTH|R|G|B|W|BRILHO+|BRILHO-`

- **Casa via IR**
  - `CASA IR ON|OFF|FLASH|STROBE|FADE|SMOOTH|R|G|B|W|BRILHO+|BRILHO-`

- **Casa (relés do arduino_som)**
  - `CASA P101010` — Parser extrai e envia `P101010` para `comandoCasa`

- **Som (Serial1)**
  - `SOM PLAY 3`, `SOM STOP`, `SOM PAUSE`, `SOM NEXT`, `SOM PREV`, `SOM VOL 20`

- **Rotina**
  - `ROTINA 3` — Executa a rotina 3

- **Faixa de sorteio**
  - `SORTEIO 1 5` — Ajusta `faixaMinima` e `faixaMaxima`

Observação: Em modo CLI, o comportamento normal (PIR/rotinas) fica pausado; a fila do som (Serial1) continua sendo processada.

## Próximas etapas
- Ajustar taxa de bits no código se necessário (`BIT_RATE`, padrão 2000 bps).
- Implementar protocolo de mensagens e fila de comandos.
- Adicionar logs/serial para depuração.
