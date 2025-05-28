#ifndef WIFIMANAGERSERVER_H
#define WIFIMANAGERSERVER_H

#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <ESPmDNS.h>  // 👈 Agregado para soporte mDNS

#define SSID_APT "NeonGuard_Config"
#define PASS_APT "12345678"
#define PIN_BOTON_RESET 4  // GPIO usado como botón tipo pull-down

class WiFiManagerServer {
public:
  WiFiManagerServer() {}

  void iniciarWiFi() {
    pinMode(PIN_BOTON_RESET, INPUT);  // Botón pull-down

    WiFiManager wifiManager;

    if (!wifiManager.autoConnect(SSID_APT, PASS_APT)) {
      Serial.println("❌ No se pudo conectar, reiniciando...");
      delay(3000);
      ESP.restart();
    }

    ipGateway = WiFi.localIP().toString();
    Serial.println("✅ Conectado a WiFi");
    Serial.print("📡 IP local: ");
    Serial.println(ipGateway);

    Preferences preferences;
    preferences.begin("wifi", false);
    preferences.putString("ssid", WiFi.SSID());
    preferences.putString("pass", WiFi.psk());
    preferences.end();

    // 🔹 Iniciar mDNS
    if (!MDNS.begin("neonguard")) {
      Serial.println("❌ Error al iniciar mDNS");
    } else {
      Serial.println("✅ mDNS iniciado: http://neonguard.local");
      MDNS.addService("http", "tcp", 80);
      MDNS.addService("ws", "tcp", 81);
    }
  }

  void mostrarCredencialesGuardadas() {
    Preferences preferences;
    preferences.begin("wifi", true);
    String ssid = preferences.getString("ssid", "No SSID");
    String pass = preferences.getString("pass", "No PASS");
    preferences.end();

    Serial.println("📂 Credenciales guardadas:");
    Serial.println("SSID: " + ssid);
    Serial.println("PASS: " + pass);
  }

  void escucharSerialParaReset() {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();

      if (input == "-1") {
        resetearCredenciales();
      }
    }

    verificarBoton();
  }

private:
  unsigned long tiempoPresionado = 0;
  bool enEspera = false;

  void verificarBoton() {
    if (digitalRead(PIN_BOTON_RESET) == HIGH) {
      if (!enEspera) {
        tiempoPresionado = millis();
        enEspera = true;
      } else if (millis() - tiempoPresionado >= 5000) {
        Serial.println("🧼 Botón mantenido 5s. Borrando configuración...");
        resetearCredenciales();
      }
    } else {
      enEspera = false;
      tiempoPresionado = 0;
    }
  }

  void resetearCredenciales() {
    Serial.println("🧹 Borrando redes guardadas y preferencias...");

    WiFiManager wifiManager;
    wifiManager.resetSettings();

    Preferences preferences;
    preferences.begin("wifi", false);
    preferences.clear();
    preferences.end();

    preferences.begin("boot", false);
    preferences.clear();
    preferences.end();

    delay(1500);
    ESP.restart();
  }
};

#endif
