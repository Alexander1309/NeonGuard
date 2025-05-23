#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "GlobalData.h"

#define SSID_APT "NeonGuard"
#define PASS_APT "12345678"

class LocalAPReceiver {
public:
  LocalAPReceiver() {}

  void iniciarAP() {
    WiFi.softAP(SSID_APT, PASS_APT);
    IPAddress IP = WiFi.softAPIP();
    ipLocal = IP.toString();
    Serial.print("📡 AP local iniciado: ");
    Serial.println(IP);

    // Ruta raíz para comprobar que el servidor está activo
    server.on("/", HTTP_GET, [this]() {
      StaticJsonDocument<128> doc;
      doc["status"] = "Servidor activo. Esperando datos...";
      doc["ip_local"] = ipLocal;
      doc["ip_gateway"] = ipGateway;

      String jsonResponse;
      serializeJson(doc, jsonResponse);
      server.send(200, "application/json", jsonResponse);
    });

    // Ruta /data para recibir POST con JSON desde la pulsera
    server.on("/data", HTTP_POST, [this]() {
      if (server.hasArg("plain")) {
        String payload = server.arg("plain");
        Serial.println("📥 Datos JSON recibidos:");
        Serial.println(payload);

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print("❌ Error al parsear JSON: ");
          Serial.println(error.c_str());
          server.send(400, "text/plain", "Error en JSON");
          return;
        }

        puls = doc["puls"];
        oxigenacion = doc["oxigenacion"];
        server.send(200, "text/plain", "Datos recibidos correctamente");
      } else {
        server.send(400, "text/plain", "No se enviaron datos");
      }
    });

    server.begin();
    Serial.println("🌐 Servidor HTTP local iniciado en /data");
  }

  void escuchar() {
    server.handleClient();  // Necesario para procesar peticiones HTTP
  }

private:
  WebServer server = WebServer(81);
};