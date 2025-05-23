#ifndef INTERNETAPISERVER_H
#define INTERNETAPISERVER_H

#include <WebServer.h>
#include <ArduinoJson.h>
#include "GlobalData.h"
#include "ForceSensor.h"
#include "LedController.h"

class InternetAPIServer {
public:
  InternetAPIServer(ForceSensor& sensorRef, LedController& ledRef)
    : sensor(sensorRef), led(ledRef) {}

  void iniciarServidor() {
    server.on("/", HTTP_GET, [this]() {
      StaticJsonDocument<128> doc;
      doc["status"] = "\U0001F310 Servidor en línea. Usa /estado para obtener datos..";
      doc["ip_local"] = ipLocal;
      doc["ip_gateway"] = ipGateway;

      String jsonResponse;
      serializeJson(doc, jsonResponse);
      server.send(200, "application/json", jsonResponse);
    });

    server.on("/estado", HTTP_GET, [this]() {
      StaticJsonDocument<128> doc;
      doc["puls"] = puls;
      doc["oxigenacion"] = oxigenacion;
      doc["promedio"] = promedio;
      doc["anomalia"] = anomalia;

      String jsonResponse;
      serializeJson(doc, jsonResponse);
      server.send(200, "application/json", jsonResponse);
    });

    server.on("/calibrar", HTTP_GET, [this]() {
      led.parpadeoLento();
      sensor.calibrate();

      StaticJsonDocument<128> doc;
      doc["status"] = "✅ Calibración iniciada correctamente.";
      String jsonResponse;
      serializeJson(doc, jsonResponse);
      server.send(200, "application/json", jsonResponse);
    });

    server.begin();
    Serial.println("🚀 Servidor HTTP (modo Internet) iniciado en /estado");
  }

  void escuchar() {
    server.handleClient();
  }

private:
  WebServer server = WebServer(80);
  ForceSensor& sensor;
  LedController& led;
};

#endif
