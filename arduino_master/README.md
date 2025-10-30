# Arduino Master (Mega)

Firmware do dispositivo Master responsável por orquestrar os efeitos e enviar comandos via rádio para os Slaves.

## Hardware
- **Placa**: Arduino Mega.
- **Módulo de rádio**: a definir.

## Arquivos
- **arduino_master.ino**: ponto de entrada do firmware do Master.

## Build e Upload
1. Abra o projeto no Arduino IDE.
2. Selecione a placa "Arduino Mega or Mega 2560" e a porta correta.
3. Compile e faça o upload para a placa.

## Próximas etapas
- Definir pinos e configuração do módulo de rádio.
- Implementar protocolo de mensagens e fila de comandos.
- Adicionar logs/serial para depuração.
