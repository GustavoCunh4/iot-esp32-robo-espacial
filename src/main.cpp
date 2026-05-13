/*
  Controle Remoto ESP32 - Robo Espacial
  Autores: Luiz Gustavo Cunha, Rafael Crusoé, Manuella Serravalle
  Simulacao: https://wokwi.com/projects/463279112690217985
*/

#include <Arduino.h>

const int pinoVRx = 34;         // Eixo X do Joystick
const int pinoVRy = 35;         // Eixo Y do Joystick
const int pinoBotao = 32;       // Botão remoto
const int pinoLedVerde = 25;    // Status: Conectado/Normal
const int pinoLedVermelho = 26; // Status: Desligado/Desconectado

// --- Constantes do Joystick ---
const int limiteSuperior = 3000;
const int limiteInferior = 1000;

// --- Variáveis de Estado ---
bool roboLigado = true;
bool ultimoEstadoBotao = HIGH;

void atualizarLEDs() {
  if (roboLigado) {
    digitalWrite(pinoLedVerde, HIGH);
    digitalWrite(pinoLedVermelho, LOW);
  } else {
    digitalWrite(pinoLedVerde, LOW);
    digitalWrite(pinoLedVermelho, HIGH);
  }
}

void lerBotaoDesligar() {
  bool estadoAtual = digitalRead(pinoBotao);

  if (ultimoEstadoBotao == HIGH && estadoAtual == LOW) {
    roboLigado = !roboLigado;

    if (!roboLigado) {
      Serial.println("Comando enviado: DESLIGAR");
      Serial.println("Status: Robô está DESLIGADO (LED Vermelho aceso).");
    } else {
      Serial.println("Comando enviado: LIGAR");
      Serial.println("Status: Robô está LIGADO (LED Verde aceso).");
    }

    atualizarLEDs();
    delay(200);
  }

  ultimoEstadoBotao = estadoAtual;
}

void processarJoystick() {
  if (!roboLigado) return;

  int valorX = analogRead(pinoVRx);
  int valorY = analogRead(pinoVRy);
  String comando = "";

  if (valorY < limiteInferior) {
    comando = "Frente";
  } else if (valorY > limiteSuperior) {
    comando = "Trás";
  } else if (valorX < limiteInferior) {
    comando = "Esquerda";
  } else if (valorX > limiteSuperior) {
    comando = "Direita";
  }

  if (comando != "") {
    Serial.println("Comando: " + comando);
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinoVRx, INPUT);
  pinMode(pinoVRy, INPUT);
  pinMode(pinoBotao, INPUT_PULLUP);
  pinMode(pinoLedVerde, OUTPUT);
  pinMode(pinoLedVermelho, OUTPUT);

  Serial.println("Iniciando Controle Remoto...");
  atualizarLEDs();
}

void loop() {
  lerBotaoDesligar();
  processarJoystick();
  delay(50);
}
