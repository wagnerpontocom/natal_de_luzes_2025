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

| Pino | Módulo              | Sinal/Conexão | Observações                |
|------|----------------------|----------------|----------------------------|
| D11  | Receptor 433 MHz     | DATA           | Antena ~17,3 cm no ANT     |
| 5V   | Receptor 433 MHz     | VCC            | —                          |
| GND  | Receptor 433 MHz     | GND            | —                          |

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

## Próximas etapas
- Definir pinos e configuração do módulo de rádio.
- Implementar tratador de mensagens e execução de efeitos.
- Adicionar logs/serial para depuração.
