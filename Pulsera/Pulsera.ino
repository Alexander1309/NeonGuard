#include "WiFiManagerPulsera.h"
#include "SensorPulsera.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define LED_PIN 2  // LED integrado en la ESP32-S3

SensorPulsera sensorPulsera;

int ultimoBPM = -1;
int ultimoSpO2 = -1;
unsigned long ultimoEnvio = 0;
const unsigned long INTERVALO_LATIDO = 5000;  // 15 segundos

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("🔌 Iniciando WiFi...");
  iniciarWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ Conectado al WiFi");
    Serial.println("🌐 IP local: " + WiFi.localIP().toString());

    IPAddress receptorIP;
    if (WiFi.hostByName("neonguard.local", receptorIP)) {
      Serial.print("✔️ IP de neonguard.local: ");
      Serial.println(receptorIP);
    } else {
      Serial.println("❌ No se pudo resolver neonguard.local");
    }
  } else {
    Serial.println("❌ Error al conectar al WiFi");
  }

  Serial.println("🔎 Iniciando sensor MAX30105...");
  sensorPulsera.iniciar();
  Serial.println("✅ Sensor listo");
}

void enviarDatos(int bpm, int spo2, bool forzado = false) {
  StaticJsonDocument<128> doc;
  doc["puls"] = bpm;
  doc["oxigenacion"] = spo2;

  String jsonStr;
  serializeJson(doc, jsonStr);

  HTTPClient http;
  http.begin("http://neonguard.local:82/pulsera");
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jsonStr);

  if (httpCode > 0) {
    if (forzado) {
      Serial.printf("🔁 Pulsera aún conectada (latido) => BPM: %d | SpO2: %d | HTTP: %d\n", bpm, spo2, httpCode);
    } else {
      Serial.printf("📤 Enviado => BPM: %d | SpO2: %d | HTTP: %d\n", bpm, spo2, httpCode);
    }
    ultimoEnvio = millis();
  } else {
    Serial.printf("❌ Error HTTP al enviar datos: %d\n", httpCode);
  }

  http.end();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    int bpm = sensorPulsera.obtenerBPM();
    int spo2 = sensorPulsera.obtenerSpO2();

    // Solo enviar si hay datos válidos Y han cambiado
    if (bpm > 0 && spo2 > 0 && (bpm != ultimoBPM || spo2 != ultimoSpO2)) {
      ultimoBPM = bpm;
      ultimoSpO2 = spo2;
      digitalWrite(LED_PIN, HIGH);
      enviarDatos(bpm, spo2);
    }

    digitalWrite(LED_PIN, LOW);

    // Si ha pasado más de 10 segundos desde el último envío, forzar latido
    if (millis() - ultimoEnvio > INTERVALO_LATIDO) {
      enviarDatos(ultimoBPM, ultimoSpO2, true);  // Enviar aunque no haya cambio
    }
  } else {
    Serial.println("🚫 WiFi desconectado. Reintentando...");
    iniciarWiFi();
  }
}
