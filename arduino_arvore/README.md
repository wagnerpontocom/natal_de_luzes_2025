# Arduino Arvore (UNO)

Firmware de um dispositivo Slave (exemplo) que recebe comandos do Master e executa efeitos de iluminação.

## Hardware
- **Placa**: Arduino UNO.
- **Módulo de rádio**: Receptor 433 MHz (compatível com ASK/OOK, ex.: XY-MK-5V).

### Pinagem (Arduino UNO → Receptor 433 MHz)
- **D11** → DATA (saída de dados do receptor para o UNO)
- **5V** → VCC
- **GND** → GND
- **Antena**: fio ~17,3 cm (1/4 de onda) no pino ANT do módulo

## Tabela de Pinos (Arduino UNO)

| Pino | Módulo/Dispositivo   | Sinal/Conexão | Observações                |
|------|-----------------------|----------------|----------------------------|
| D11  | Receptor 433 MHz      | DATA           | Antena ~17,3 cm no ANT     |
| 5V   | Receptor 433 MHz      | VCC            | —                          |
| GND  | Receptor 433 MHz      | GND            | —                          |
| D2   | Módulo de Relés (CH1) | Relé 1         | Saída digital (LOW=desl., HIGH=lig.) |
| D3   | Módulo de Relés (CH2) | Relé 2         | Saída digital              |
| D4   | Módulo de Relés (CH3) | Relé 3         | Saída digital              |
| D5   | Módulo de Relés (CH4) | Relé 4         | Saída digital              |
| D6   | Módulo de Relés (CH5) | Relé 5         | Saída digital              |
| D7   | Módulo de Relés (CH6) | Relé 6         | Saída digital              |

## Arquivos
- **arduino_arvore.ino**: ponto de entrada do firmware do Slave de exemplo.

## Build e Upload
1. Abra o projeto no Arduino IDE.
2. Selecione a placa "Arduino UNO" e a porta correta.
3. Compile e faça o upload para a placa.

## Observações
- Biblioteca necessária: RadioHead (RH_ASK). Instale pelo Library Manager do Arduino IDE.
- A taxa de bits deve coincidir com o Master: `BIT_RATE = 2000` bps (ajustável em ambos se necessário).

## Mensagens RF suportadas
- `11000` → imprime: `árvore low`
- `01111` → imprime: `árvore high`
- Outros conteúdos → imprime `Recebido: <mensagem>`

## Protocolo Serial (RX)
- Mensagem de 7 caracteres iniciando com `P`.
- Formato: `Pabcdef` onde `a`..`f` ∈ {`0`,`1`} controlam os 6 relés, em ordem do menor pino para o maior: Relé1→D2, Relé2→D3, Relé3→D4, Relé4→D5, Relé5→D6, Relé6→D7.
- `1` = liga o relé correspondente. `0` = desliga.

### Exemplos
- `P001001` → Relé 3 e Relé 6 ligados; demais desligados.
- `P100000` → somente Relé 1 ligado.
- `P000001` → somente Relé 6 ligado.
- `P111111` → todos ligados.
- `P000000` → todos desligados.

## Próximas etapas
- Definir pinos e configuração do módulo de rádio.
- Implementar tratador de mensagens e execução de efeitos.
- Adicionar logs/serial para depuração.
