#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <UrlEncode.h>
#include <ESP32Servo.h>

// ==========================
// MAPEAMENTO DE PINOS
// ==========================

// Sensor de luminosidade
const int pinoLDR = 2;

// Sensor ultrassônico HC-SR04
const int pinoTrig = 12;
const int pinoEcho = 13;

// LEDs
const int pinoLEDVerde = 5;
const int pinoLEDVermelho = 3;

// Servo motor
const int pinoServo = 18;

// Joystick
const int pinoJoyX = 4;
const int pinoJoyY = 1;

// ==========================
// OBJETO DO SERVO
// ==========================

Servo servoMotor;

// ==========================
// CONFIGURAÇÕES DE REDE
// ==========================

const char* ssid = "Rafa Crusoé";
const char* password = "12345689";

// ==========================
// CALLMEBOT
// ==========================

String phoneNumber = "557191156488";
String apiKey = "5262726";

// ==========================
// SUPABASE
// ==========================

String supabaseUrl = "https://chzipdtmlyvsrxoijelj.supabase.co/rest/v1/leituras";
String supabaseKey = "sb_publishable_Tg4BpQK7kxQ9qRi7B43Eug_-WqNzjIF";

// ==========================
// VARIÁVEIS DE CONTROLE
// ==========================

unsigned long tempoUltimaLeitura = 0;
unsigned long tempoUltimaTentativaWiFi = 0;

bool emAlerta = false;
bool objetoDetectadoAtual = false;

int anguloServo = 90;
int valorJoyX = 0;
int valorJoyY = 0;

const int limiteLuz = 2000;
const float limiteDistanciaCm = 30.0;

const unsigned long intervaloReconexaoWiFi = 5000;

// ==========================
// FUNÇÃO PARA LER DISTÂNCIA
// ==========================

float medirDistanciaCm() {
  digitalWrite(pinoTrig, LOW);
  delayMicroseconds(2);

  digitalWrite(pinoTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrig, LOW);

  long duracao = pulseIn(pinoEcho, HIGH, 30000);

  if (duracao == 0) {
    return -1;
  }

  float distancia = duracao / 58.0;
  return distancia;
}

// ==========================
// WI-FI
// ==========================

bool wifiConectado() {
  return WiFi.status() == WL_CONNECTED;
}

void verificarWiFi() {
  if (!wifiConectado()) {
    digitalWrite(pinoLEDVerde, LOW);
    digitalWrite(pinoLEDVermelho, HIGH);

    if (millis() - tempoUltimaTentativaWiFi >= intervaloReconexaoWiFi) {
      tempoUltimaTentativaWiFi = millis();

      Serial.println("Wi-Fi desconectado!");
      Serial.println("LED vermelho aceso.");
      Serial.println("Tentando reconectar ao Wi-Fi...");

      WiFi.disconnect();
      WiFi.begin(ssid, password);
    }
  }
}

// ==========================
// CALLMEBOT
// ==========================

void sendMessage(String message) {
  if (!wifiConectado()) {
    Serial.println("Wi-Fi desconectado. Não foi possível enviar mensagem via CallmeBot.");
    return;
  }

  String url = "https://api.callmebot.com/whatsapp.php?phone="
               + phoneNumber
               + "&apikey="
               + apiKey
               + "&text="
               + urlEncode(message);

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, url);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    Serial.println("CallmeBot: mensagem enviada com sucesso via WhatsApp!");
  } else {
    Serial.print("CallmeBot: erro ao enviar mensagem. HTTP code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

// ==========================
// SUPABASE
// ==========================

void enviarLeituraSupabase(
  int luminosidade,
  float distanciaCm,
  bool objetoProximo,
  int probabilidade,
  int joyX,
  int joyY,
  int angulo,
  String estadoRobo,
  String estadoLEDVerde,
  String estadoLEDVermelho,
  String statusWifi
) {
  if (!wifiConectado()) {
    Serial.println("Supabase: Wi-Fi desconectado. Dados não enviados.");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  http.begin(client, supabaseUrl);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", supabaseKey);
  http.addHeader("Authorization", "Bearer " + supabaseKey);
  http.addHeader("Prefer", "return=representation");

  String json = "{";
  json += "\"luminosidade\":" + String(luminosidade) + ",";
  json += "\"probabilidade_vida\":" + String(probabilidade) + ",";
  json += "\"distancia_cm\":" + String(distanciaCm, 2) + ",";
  json += "\"objeto_proximo\":" + String(objetoProximo ? "true" : "false") + ",";
  json += "\"joy_x\":" + String(joyX) + ",";
  json += "\"joy_y\":" + String(joyY) + ",";
  json += "\"angulo_servo\":" + String(angulo) + ",";
  json += "\"estado_robo\":\"" + estadoRobo + "\",";
  json += "\"led_verde\":\"" + estadoLEDVerde + "\",";
  json += "\"led_vermelho\":\"" + estadoLEDVermelho + "\",";
  json += "\"wifi_status\":\"" + statusWifi + "\"";
  json += "}";

  Serial.println();
  Serial.println("---------- ENVIO PARA SUPABASE ----------");
  Serial.println("JSON enviado:");
  Serial.println(json);

  int httpResponseCode = http.POST(json);

  Serial.print("Código HTTP Supabase: ");
  Serial.println(httpResponseCode);

  String resposta = http.getString();

  Serial.println("Resposta do Supabase:");
  Serial.println(resposta);

  if (httpResponseCode == 201) {
    Serial.println("Supabase: leitura salva com sucesso!");
  } else {
    Serial.println("Supabase: erro ao salvar leitura.");
  }

  Serial.println("-----------------------------------------");
  Serial.println();

  http.end();
}

// ==========================
// CONTROLE DO SERVO PELO JOYSTICK
// ==========================

void controlarServoPeloJoystick() {
  valorJoyX = analogRead(pinoJoyX);
  valorJoyY = analogRead(pinoJoyY);

  int novoAngulo = map(valorJoyX, 0, 4095, 0, 180);
  novoAngulo = constrain(novoAngulo, 0, 180);

  anguloServo = novoAngulo;
  servoMotor.write(anguloServo);
}

// ==========================
// SETUP
// ==========================

void setup() {
  Serial.begin(115200);

  pinMode(pinoLDR, INPUT);

  pinMode(pinoTrig, OUTPUT);
  pinMode(pinoEcho, INPUT);

  pinMode(pinoLEDVerde, OUTPUT);
  pinMode(pinoLEDVermelho, OUTPUT);

  servoMotor.attach(pinoServo);
  servoMotor.write(90);

  digitalWrite(pinoLEDVerde, LOW);
  digitalWrite(pinoLEDVermelho, HIGH);

  Serial.println("Sistema iniciado.");
  Serial.println("Componentes ativos: LDR, ultrassonico, joystick, servo, LEDs, Wi-Fi, CallmeBot e Supabase.");
  Serial.println();

  WiFi.begin(ssid, password);
  Serial.println("Conectando ao Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(pinoLEDVerde, LOW);
    digitalWrite(pinoLEDVermelho, HIGH);

    delay(500);
    Serial.print(".");
  }

  digitalWrite(pinoLEDVerde, HIGH);
  digitalWrite(pinoLEDVermelho, LOW);

  Serial.println();
  Serial.println("Wi-Fi conectado!");
  Serial.println("LED verde aceso.");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

// ==========================
// LOOP PRINCIPAL
// ==========================

void loop() {
  verificarWiFi();

  controlarServoPeloJoystick();

  if (millis() - tempoUltimaLeitura >= 2000) {
    tempoUltimaLeitura = millis();

    int intensidadeLuz = analogRead(pinoLDR);
    float distanciaCm = medirDistanciaCm();

    if (distanciaCm > 0 && distanciaCm <= limiteDistanciaCm) {
      objetoDetectadoAtual = true;
    } else {
      objetoDetectadoAtual = false;
    }

    int probabilidade = 0;

    if (intensidadeLuz > limiteLuz) {
      probabilidade += 40;
    }

    if (objetoDetectadoAtual) {
      probabilidade += 60;
    }

    String estadoRobo;
    String estadoLEDVerde;
    String estadoLEDVermelho;

    bool wifiOk = wifiConectado();

    if (!wifiOk) {
      digitalWrite(pinoLEDVerde, LOW);
      digitalWrite(pinoLEDVermelho, HIGH);

      estadoRobo = "ERRO DE WIFI";
      estadoLEDVerde = "APAGADO";
      estadoLEDVermelho = "ACESO";
    }
    else if (probabilidade <= 75) {
      emAlerta = false;

      digitalWrite(pinoLEDVerde, HIGH);
      digitalWrite(pinoLEDVermelho, LOW);

      estadoRobo = "EXPLORACAO NORMAL";
      estadoLEDVerde = "ACESO";
      estadoLEDVermelho = "APAGADO";
    }
    else {
      digitalWrite(pinoLEDVerde, LOW);
      digitalWrite(pinoLEDVermelho, HIGH);

      estadoRobo = "ALERTA";
      estadoLEDVerde = "APAGADO";
      estadoLEDVermelho = "ACESO";

      if (!emAlerta) {
        emAlerta = true;
        sendMessage("Alerta! Alta probabilidade de vida detectada no planeta.");
      }
    }

    Serial.println("========== LEITURA DO SISTEMA ==========");

    Serial.println("ROBO");
    Serial.println("Estado do robo: LIGADO");
    Serial.print("Modo atual: ");
    Serial.println(estadoRobo);

    Serial.println();

    Serial.println("LDR / SENSOR DE LUZ");
    Serial.print("Valor bruto do LDR: ");
    Serial.println(intensidadeLuz);
    Serial.print("Limite configurado: ");
    Serial.println(limiteLuz);
    Serial.print("Condicao de luz: ");
    Serial.println(intensidadeLuz > limiteLuz ? "Luz adequada" : "Luz baixa ou inadequada");

    Serial.println();

    Serial.println("SENSOR ULTRASSONICO");
    Serial.print("Distancia medida: ");

    if (distanciaCm < 0) {
      Serial.println("Sem leitura valida");
    } else {
      Serial.print(distanciaCm, 2);
      Serial.println(" cm");
    }

    Serial.print("Limite de deteccao: ");
    Serial.print(limiteDistanciaCm);
    Serial.println(" cm");

    Serial.print("Objeto proximo: ");
    Serial.println(objetoDetectadoAtual ? "SIM" : "NAO");

    Serial.println();

    Serial.println("JOYSTICK");
    Serial.print("Valor eixo X: ");
    Serial.println(valorJoyX);
    Serial.print("Valor eixo Y: ");
    Serial.println(valorJoyY);

    Serial.println();

    Serial.println("SERVO MOTOR");
    Serial.print("Angulo atual do servo: ");
    Serial.print(anguloServo);
    Serial.println(" graus");

    Serial.println();

    Serial.println("LEDS");
    Serial.print("LED verde: ");
    Serial.println(estadoLEDVerde);
    Serial.print("LED vermelho: ");
    Serial.println(estadoLEDVermelho);

    Serial.println();

    Serial.println("WI-FI");
    Serial.print("Status: ");
    Serial.println(wifiOk ? "CONECTADO" : "DESCONECTADO");
    Serial.print("IP: ");
    if (wifiOk) {
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("Sem IP");
    }

    Serial.println();

    Serial.println("PROBABILIDADE DE VIDA");
    Serial.print("Probabilidade calculada: ");
    Serial.print(probabilidade);
    Serial.println("%");

    if (!wifiOk) {
      Serial.println("Mensagem: Wi-Fi desconectado. LED vermelho aceso.");
    }
    else if (probabilidade <= 75) {
      Serial.println("Mensagem: Exploracao normal. Nenhum indicio relevante detectado.");
    } else {
      Serial.println("Mensagem: ALERTA! Alta probabilidade de vida detectada!");
    }

    Serial.println("========================================");
    Serial.println();

    enviarLeituraSupabase(
      intensidadeLuz,
      distanciaCm,
      objetoDetectadoAtual,
      probabilidade,
      valorJoyX,
      valorJoyY,
      anguloServo,
      estadoRobo,
      estadoLEDVerde,
      estadoLEDVermelho,
      wifiOk ? "CONECTADO" : "DESCONECTADO"
    );
  }
}