#include "GlobalData.h"
#include "WiFiManagerServer.h"
#include "LocalAPReceiver.h"
#include "InternetAPIServer.h"

// Instancias
WiFiManagerServer wifiManager;
LocalAPReceiver apReceiver;
InternetAPIServer internetAPI;

bool conectadoInternet = false;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void tareaWiFiInternet(void* parameter) {
  wifiManager.iniciarWiFi();
  wifiManager.mostrarCredencialesGuardadas();

  // Iniciar servidor de datos por internet
  internetAPI.iniciarServidor();

  // Avisar que ya hay conexión
  portENTER_CRITICAL(&mux);
  conectadoInternet = true;
  portEXIT_CRITICAL(&mux);

  while (true) {
    internetAPI.escuchar(); // Mantenemos el servidor corriendo
    delay(10); // Pequeño delay para evitar saturación
  }
}

void tareaRedLocal(void* parameter) {
  // Esperar hasta tener conexión a Internet
  while (true) {
    bool acceso;
    portENTER_CRITICAL(&mux);
    acceso = conectadoInternet;
    portEXIT_CRITICAL(&mux);

    if (acceso) {
      break;
    }

    Serial.println("[Core 1] Esperando conexión a Internet...");
    delay(1000);
  }

  apReceiver.iniciarAP();

  while (true) {
    apReceiver.escuchar(); // Servidor en modo AP sigue atendiendo
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(
    tareaWiFiInternet,
    "WiFiTarea",
    10000,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    tareaRedLocal,
    "RedLocalTarea",
    10000,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  wifiManager.escucharSerialParaReset();
}
