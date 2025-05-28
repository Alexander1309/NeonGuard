// WiFiManagerPulsera.h
#ifndef WIFIMANAGERPULSERA_H
#define WIFIMANAGERPULSERA_H

#include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager

void iniciarWiFi() {
  WiFiManager wm;
  wm.setDebugOutput(true);
  bool res;

  res = wm.autoConnect("NeonGuard_Pulsera", "12345678");

  if (!res) {
    Serial.println("❌ Falló la conexión o configuración. Reiniciando...");
    ESP.restart();
  } else {
    Serial.print("✅ Conectado a: ");
    Serial.println(WiFi.SSID());
    Serial.print("📡 IP asignada: ");
    Serial.println(WiFi.localIP());
  }
}

#endif
