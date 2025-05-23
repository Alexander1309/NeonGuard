#include <WebServer.h>
#include <ArduinoJson.h>
#include "GlobalData.h"

class InternetAPIServer {
  public:
    InternetAPIServer() {}

    void iniciarServidor() {
      server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/plain", "🌐 Servidor en línea. Usa /estado para obtener datos.");
      });

      server.on("/estado", HTTP_GET, [this]() {
        StaticJsonDocument<128> doc;
        doc["puls"] = puls;
        doc["movimiento"] = movimiento;
        doc["oxigenacion"] = oxigenacion;

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
};