# Natal2025

Projeto de automação de luzes natalinas com múltiplos Arduinos comunicando-se via rádio. Um Arduino Master (Mega) envia comandos para vários Arduinos Slaves (UNO), que executam efeitos de iluminação.

## Arquitetura
- **Master (Arduino Mega)**: central de controle, envia comandos.
- **Slaves (Arduino UNO)**: executam comandos recebidos do Master.
- **Comunicação**: via rádio entre os dispositivos (módulo específico a definir em etapas futuras).

## Estrutura do repositório
- **arduino_master/**: firmware do Master (Arduino Mega).
- **arduino_arvore/**: firmware de um Slave de exemplo (Arduino UNO). Novos Slaves seguirão este padrão.

## Pré-requisitos
- **Arduino IDE** ou **Arduino CLI**.
- Placas: Arduino Mega (Master) e Arduino UNO (Slaves).
- Módulos de rádio compatíveis (a definir).

## Próximos passos
- **Definir módulo de rádio** (p.ex. NRF24L01, HC-12, etc.).
- **Especificar protocolo de mensagens** Master ⇄ Slaves.
- **Implementar firmware do Master e dos Slaves** conforme requisitos.

## Como contribuir
- Cada dispositivo terá seu README próprio com instruções específicas.
- Sugestões de arquitetura e protocolo são bem-vindas.
