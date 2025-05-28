#ifndef INTERNETAPISERVER_H
#define INTERNETAPISERVER_H

#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "GlobalData.h"
#include "ForceSensor.h"
#include "LedController.h"

class InternetAPIServer {
public:
  InternetAPIServer(ForceSensor& sensorRef, LedController& ledRef)
    : sensor(sensorRef), led(ledRef), webSocket(81) {}

  void iniciarServidor() {
    webSocket.begin();
    webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
      if (type == WStype_TEXT) {
        String mensaje = String((char*)payload);
        
        if (mensaje == "getEstado") {
          enviarEstado(num);
        } else if (mensaje == "calibrar") {
          led.parpadeoLento();
          sensor.calibrate(100, 10);

          StaticJsonDocument<64> resp;
          resp["status"] = "✅ Calibración completada.";
          String json;
          serializeJson(resp, json);
          webSocket.sendTXT(num, json);
        }
      }
    });

    Serial.println("📡 WebSocket iniciado en puerto 81");
  }

  void escuchar() {
    webSocket.loop();
  }

private:
  WebSocketsServer webSocket;
  ForceSensor& sensor;
  LedController& led;

  void enviarEstado(uint8_t clientID) {
    StaticJsonDocument<128> doc;
    doc["puls"] = puls;
    doc["oxigenacion"] = oxigenacion;
    doc["promedio"] = promedio;
    doc["anomalia"] = anomalia;

    String json;
    serializeJson(doc, json);
    webSocket.sendTXT(clientID, json);
  }
};

#endif