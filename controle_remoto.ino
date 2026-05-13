const int pinoVRx = 34;
const int pinoVRy = 35;
const int pinoBotao = 32;
const int pinoLedVerde = 25;
const int pinoLedVermelho = 26;

const int limiteSuperior = 3000;
const int limiteInferior = 1000;

bool roboLigado = true;
bool ultimoEstadoBotao = HIGH;

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

void lerBotaoDesligar() {
  bool estadoAtual = digitalRead(pinoBotao);

  if (ultimoEstadoBotao == HIGH && estadoAtual == LOW) {
    roboLigado = !roboLigado;

    if (!roboLigado) {
      Serial.println("Comando enviado: DESLIGAR");
      Serial.println("Status: Robo esta DESLIGADO (LED Vermelho aceso).");
    } else {
      Serial.println("Comando enviado: LIGAR");
      Serial.println("Status: Robo esta LIGADO (LED Verde aceso).");
    }

    atualizarLEDs();
    delay(200);
  }

  ultimoEstadoBotao = estadoAtual;
}

void atualizarLEDs() {
  if (roboLigado) {
    digitalWrite(pinoLedVerde, HIGH);
    digitalWrite(pinoLedVermelho, LOW);
  } else {
    digitalWrite(pinoLedVerde, LOW);
    digitalWrite(pinoLedVermelho, HIGH);
  }
}

void processarJoystick() {
  if (!roboLigado) return;

  int valorX = analogRead(pinoVRx);
  int valorY = analogRead(pinoVRy);
  String comando = "";

  if (valorY < limiteInferior) {
    comando = "Frente";
  } else if (valorY > limiteSuperior) {
    comando = "Tras";
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
