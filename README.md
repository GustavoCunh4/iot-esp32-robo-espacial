# iot-esp32-robo-espacial

## Nome do Projeto
Controle Remoto com Joystick para ESP32 — Robô Espacial

## Autores
- Luiz Gustavo Cunha
- Rafael Crusoé
- Manuella Serravalle

## Objetivo da Etapa
Desenvolver o firmware do controle remoto do robô espacial utilizando ESP32. O controle lê as entradas de um joystick analógico e de um botão, envia comandos de direção pela serial e indica o estado do robô (ligado/desligado) por meio de LEDs.

## Componentes do Circuito

| Componente | Quantidade | Pino ESP32 |
|---|---|---|
| ESP32 DevKit C V4 | 1 | — |
| Joystick Analógico | 1 | HORZ → 34, VERT → 35 |
| Botão (Push Button) | 1 | 32 |
| LED Verde | 1 | 25 |
| LED Vermelho | 1 | 26 |
| Protoboard | 1 | — |

## Como Rodar no Wokwi

1. Acesse o link do projeto: [https://wokwi.com/projects/463279112690217985](https://wokwi.com/projects/463279112690217985)
2. Clique em **Play** (botão verde) para iniciar a simulação
3. Abra o **Serial Monitor** no canto inferior da tela
4. Interaja com o joystick: mova para cima, baixo, esquerda ou direita
5. Pressione o botão verde para ligar/desligar o robô
6. Observe os comandos no Serial Monitor e os LEDs indicando o estado

## Funcionamento

- **LED Verde aceso** → Robô ligado, aguardando comandos
- **LED Vermelho aceso** → Robô desligado, joystick ignorado
- **Botão** → Alterna entre ligado e desligado (debounce de 200ms)
- **Joystick** → Envia comandos: `Frente`, `Tras`, `Esquerda`, `Direita`
