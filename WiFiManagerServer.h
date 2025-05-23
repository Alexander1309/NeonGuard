#ifndef WIFIMANAGERSERVER_H
#define WIFIMANAGERSERVER_H

#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>

#define SSID_APT "NeonGuard_Config"
#define PASS_APT "12345678"

class WiFiManagerServer {
public:
  WiFiManagerServer() {}

  void iniciarWiFi() {
    manejarReinicios();

    WiFiManager wifiManager;

    if (!wifiManager.autoConnect(SSID_APT, PASS_APT)) {
      Serial.println("❌ No se pudo conectar, reiniciando...");
      delay(3000);
      ESP.restart();
    }

    Serial.println("✅ Conectado a WiFi");
    Serial.print("📡 IP local: ");
    Serial.println(WiFi.localIP());

    Preferences preferences;
    preferences.begin("wifi", false);
    preferences.putString("ssid", WiFi.SSID());
    preferences.putString("pass", WiFi.psk());
    preferences.end();
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
  }

private:
  void manejarReinicios() {
    Preferences prefs;
    prefs.begin("boot", false);

    unsigned long lastBoot = prefs.getULong("lastBootTime", 0);
    int bootCount = prefs.getInt("bootCount", 0);
    unsigned long now = millis() / 1000;  // Segundos desde arranque

    // Si el último boot fue hace menos de 30 segundos, contar como reinicio rápido
    if (now - lastBoot < 30) {
      bootCount++;
    } else {
      bootCount = 1; // Resetear si ha pasado mucho tiempo
    }

    prefs.putULong("lastBootTime", now);
    prefs.putInt("bootCount", bootCount);
    prefs.end();

    if (bootCount >= 3) {
      Serial.println("⚠️ Reinicio rápido x3 detectado. Borrando credenciales...");
      resetearCredenciales();
    }
  }

  void resetearCredenciales() {
    Serial.println("🧹 Borrando redes guardadas y preferencias...");

    // Borrar configuración WiFi
    WiFiManager wifiManager;
    wifiManager.resetSettings();

    // Borrar preferencias
    Preferences preferences;
    preferences.begin("wifi", false);
    preferences.clear();
    preferences.end();

    // También limpiar el contador de reinicios
    preferences.begin("boot", false);
    preferences.clear();
    preferences.end();

    delay(1500);
    ESP.restart();
  }
};

#endif
