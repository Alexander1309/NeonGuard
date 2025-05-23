#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <Arduino.h>

class LedController {
public:
  LedController(uint8_t pin) : pin(pin) {}

  void iniciar() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  void actualizar() {
    if (modo == PARPADEO_LENTO || modo == PARPADEO_LARGO || modo == PARPADEO_INFINITO || modo == TRES_DESTELLOS) {
      unsigned long ahora = millis();
      if (ahora - ultimoCambio >= intervalo) {
        ultimoCambio = ahora;
        estado = !estado;

        if (modo == TRES_DESTELLOS && parpadeosRealizados >= 6) {
          apagar();
          return;
        }

        digitalWrite(pin, estado ? HIGH : LOW);

        if (modo == TRES_DESTELLOS)
          parpadeosRealizados++;
      }
    }
  }

  void encender() {
    reiniciar();
    modo = ENCENDIDO;
    digitalWrite(pin, HIGH);
  }

  void apagar() {
    reiniciar();
    modo = APAGADO;
    digitalWrite(pin, LOW);
  }

  void tresDestellos() {
    reiniciar();
    modo = TRES_DESTELLOS;
    intervalo = 300;
    parpadeosRealizados = 0;
    estado = false;
    ultimoCambio = millis();
  }

  void parpadeoLento() {
    reiniciar();
    modo = PARPADEO_LENTO;
    intervalo = 800;
    estado = false;
    ultimoCambio = millis();
  }

  void parpadeoLargo() {
    reiniciar();
    modo = PARPADEO_LARGO;
    intervalo = 1500;
    estado = false;
    ultimoCambio = millis();
  }

  void parpadeoInfinito(unsigned long velocidad = 500) {
    reiniciar();
    modo = PARPADEO_INFINITO;
    intervalo = velocidad;
    estado = false;
    ultimoCambio = millis();
  }

private:
  enum Modo { APAGADO, ENCENDIDO, PARPADEO_LENTO, PARPADEO_LARGO, PARPADEO_INFINITO, TRES_DESTELLOS };
  Modo modo = APAGADO;

  uint8_t pin;
  bool estado = false;
  unsigned long ultimoCambio = 0;
  unsigned long intervalo = 500;
  uint8_t parpadeosRealizados = 0;

  void reiniciar() {
    estado = false;
    parpadeosRealizados = 0;
    digitalWrite(pin, LOW);
  }
};

#endif