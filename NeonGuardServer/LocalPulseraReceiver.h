#ifndef LOCALPULSERARECEIVER_H
#define LOCALPULSERARECEIVER_H

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "GlobalData.h"

class LocalPulseraReceiver {
public:
  LocalPulseraReceiver() : server(82), ultimaRecepcion(0) {}

  void iniciarServidor() {
    // Ruta para recibir datos desde la pulsera
    server.on("/pulsera", HTTP_POST, [this]() {
      if (server.hasArg("plain")) {
        String dataStr = server.arg("plain");
        Serial.println("📥 Datos JSON HTTP recibidos:");
        Serial.println(dataStr);

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, dataStr);

        if (error) {
          Serial.print("❌ Error al parsear JSON: ");
          Serial.println(error.c_str());
          pulcera_con = false;
          server.send(400, "application/json", "{\"status\":\"error\",\"reason\":\"Invalid JSON\"}");
          return;
        }

        // ✅ Datos válidos
        puls = doc["puls"];
        oxigenacion = doc["oxigenacion"];
        pulcera_con = true;
        ultimaRecepcion = millis();

        server.send(200, "application/json", "{\"status\":\"ok\"}");
      } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"reason\":\"No body\"}");
      }
    });

    server.begin();
    Serial.println("📡 Servidor HTTP receptor de pulsera iniciado en puerto 82");
  }

  void escuchar() {
    server.handleClient();

    // 🔻 Verificar timeout de recepción
    const unsigned long TIMEOUT_PULSERA_MS = 30000;  // 30 segundos
    if (pulcera_con && millis() - ultimaRecepcion > TIMEOUT_PULSERA_MS) {
      Serial.println("⚠️ Tiempo sin datos de la pulsera. Se considera desconectada.");
      pulcera_con = false;
    }
  }

private:
  WebServer server;
  unsigned long ultimaRecepcion;
};

#endif
