#include "GlobalData.h"
#include "WiFiManagerServer.h"
#include "InternetAPIServer.h"
#include "ForceSensor.h"
#include "LedController.h"
#include "LocalPulseraReceiver.h"  // 🔹 Receptor vía HTTP

// Instancias
LedController led(2);
ForceSensor sensores;
WiFiManagerServer wifiManager;
InternetAPIServer internetAPI(sensores, led);
LocalPulseraReceiver receptorPulsera;  // 🔹 Servidor HTTP para datos de la pulsera

// Variables de control
unsigned long tiempoAnterior = 0;
const unsigned long intervaloLectura = 200;
bool conectadoInternet = false;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
bool servidorPulseraIniciado = false;

// 🧠 Tarea Core 0: Conexión WiFi y servidores
void tareaWiFiServidor(void* parameter) {
  led.parpadeoLargo();

  wifiManager.iniciarWiFi();
  wifiManager.mostrarCredencialesGuardadas();

  internetAPI.iniciarServidor();  // WebSocket para interfaz o dashboard

  portENTER_CRITICAL(&mux);
  conectadoInternet = true;
  portEXIT_CRITICAL(&mux);

  while (true) {
    internetAPI.escuchar();  // WebSocket loop

    // 🔹 Inicia el servidor HTTP receptor solo una vez
    if (WiFi.status() == WL_CONNECTED && !servidorPulseraIniciado) {
      receptorPulsera.iniciarServidor();
      servidorPulseraIniciado = true;
    }

    if (servidorPulseraIniciado) {
      receptorPulsera.escuchar();  // HTTP loop
    }

    delay(10);
  }
}

// 🧠 Tarea Core 1: Lectura de sensores y lógica de IA
void tareaSensores(void* parameter) {
  while (true) {
    bool listo;
    portENTER_CRITICAL(&mux);
    listo = conectadoInternet;
    portEXIT_CRITICAL(&mux);

    if (listo && (millis() - tiempoAnterior >= intervaloLectura)) {
      tiempoAnterior = millis();
      led.encender();

      sensores.printSensorValues();
      promedio = sensores.getAverageSignal();
      Serial.print("📊 Promedio: ");
      Serial.println(promedio);

      int bpm = puls;
      int spo2 = oxigenacion;
      Serial.println("❤️ Ritmo cardiaco: " + String(bpm));
      Serial.println("🩸 Oxigenación: " + String(spo2) + "%");

      anomalia = sensores.predictAnomaly(bpm, spo2);
      Serial.print("🚨 Anomalía estimada: ");
      Serial.print(anomalia);
      Serial.println("%");
    }

    delay(10);
  }
}

void setup() {
  Serial.begin(115200);
  sensores.begin();
  led.iniciar();
  delay(1000);

  xTaskCreatePinnedToCore(
    tareaWiFiServidor,
    "WiFiServidor",
    10000,
    NULL,
    1,
    NULL,
    0);

  xTaskCreatePinnedToCore(
    tareaSensores,
    "LecturaSensores",
    10000,
    NULL,
    1,
    NULL,
    1);
}

void loop() {
  wifiManager.escucharSerialParaReset();
  led.actualizar();
}
