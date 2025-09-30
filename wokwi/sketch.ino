#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ====== Wi-Fi Wokwi ======
const char* SSID = "Wokwi-GUEST";
const char* PASSWORD = "";

// ====== Broker MQTT ======
const char* BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char* ID_MQTT = "esp32_mottuspot";
const char* TOPIC_SUBSCRIBE = "iot/mottuspot/comando";
const char* TOPIC_PUBLISH   = "iot/mottuspot/status";

// ====== Pinos ======
#define LED_PIN 2
#define BUZZER_PIN 4

// ====== Estado do localizador ======
bool localizadorAtivo = false;

// ====== WiFi e MQTT ======
WiFiClient espClient;
PubSubClient client(espClient);

// ====== Conecta ao Wi-Fi ======
void setupWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi conectado");
}

// ====== Publica status no MQTT ======
void publicarStatus(const char* placa, const char* descricao, const char* status) {
  StaticJsonDocument<256> doc;
  doc["placa"] = placa;
  doc["descricao"] = descricao;
  doc["status"] = status;

  char buffer[256];
  size_t n = serializeJson(doc, buffer);
  client.publish(TOPIC_PUBLISH, buffer, n);
}

// ====== Liga LED e Buzzer ======
void ligarAlerta() {
  if (!localizadorAtivo) {
    localizadorAtivo = true;
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 500);
    Serial.println("🔊 Alerta ativado");
    publicarStatus("ABC-1234", "Moto vermelha", "ativado");
  }
}

// ====== Desliga LED e Buzzer ======
void desligarAlerta() {
  if (localizadorAtivo) {
    localizadorAtivo = false;
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
    Serial.println("✅ Alerta desativado");
    publicarStatus("ABC-1234", "Moto vermelha", "desativado");
  }
}

// ====== Callback de mensagem MQTT ======
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);

  if (err) {
    Serial.println("⚠️ Erro ao decodificar JSON");
    return;
  }

  const char* placa = doc["placa"];
  const char* descricao = doc["descricao"];
  const char* status = doc["status"];

  Serial.println("🚨 Comando recebido:");
  Serial.print("Placa: ");
  Serial.println(placa);
  Serial.print("Descrição: ");
  Serial.println(descricao);
  Serial.print("Status: ");
  Serial.println(status);

  if (String(status) == "ativar") {
    ligarAlerta();
  } else if (String(status) == "desativar") {
    desligarAlerta();
  }
}

// ====== Conecta ao broker MQTT ======
void setupMQTT() {
  client.setServer(BROKER_MQTT, BROKER_PORT);
  client.setCallback(callbackMQTT);
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect(ID_MQTT)) {
      Serial.println(" conectado!");
      client.subscribe(TOPIC_SUBSCRIBE);
    } else {
      Serial.print(" falhou. Estado: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  desligarAlerta();

  setupWiFi();
  setupMQTT();
}

// ====== Loop principal ======
void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  // Pisca buzzer se estiver ativo
  if (localizadorAtivo) {
    static unsigned long lastToggle = 0;
    static bool buzzerOn = true;
    unsigned long now = millis();

    if (buzzerOn && now - lastToggle > 300) {
      noTone(BUZZER_PIN);
      buzzerOn = false;
      lastToggle = now;
    } else if (!buzzerOn && now - lastToggle > 700) {
      tone(BUZZER_PIN, 500);
      buzzerOn = true;
      lastToggle = now;
    }
  }
}
