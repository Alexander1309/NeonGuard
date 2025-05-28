#ifndef LOCALPULSERARECEIVER_H
#define LOCALPULSERARECEIVER_H

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "GlobalData.h"

class LocalPulseraReceiver {
public:
  LocalPulseraReceiver() : webSocket(82) {}  // Puerto 82 para recibir datos desde la pulsera

  void iniciarServidor() {
    // Iniciar WebSocket para recibir datos de la pulsera
    webSocket.begin();
    webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
      if (type == WStype_TEXT) {
        String dataStr = String((char *)payload);
        Serial.println("📥 Datos JSON WebSocket recibidos:");
        Serial.println(dataStr);

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, dataStr);

        if (error) {
          Serial.print("❌ Error al parsear JSON: ");
          Serial.println(error.c_str());
          return;
        }

        puls = doc["puls"];
        oxigenacion = doc["oxigenacion"];
      }
    });

    Serial.println("📡 WebSocket receptor de pulsera iniciado en puerto 82");
  }

  void escuchar() {
    webSocket.loop();  // Procesar conexiones WebSocket
  }

private:
  WebSocketsServer webSocket;
};

#endif
