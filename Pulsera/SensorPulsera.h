// SensorPulsera.h
#ifndef SENSORPULSERA_H
#define SENSORPULSERA_H

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

class SensorPulsera {
private:
  MAX30105 sensor;
  uint32_t irBuffer[100];
  uint32_t redBuffer[100];
  int32_t spo2;
  int8_t validSPO2;
  int32_t heartRate;
  int8_t validHeartRate;

public:
  void iniciar() {
    Wire.begin(1, 2);
    if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
      Serial.println("❌ MAX30105 no encontrado.");
      while (1)
        ;
    }

    sensor.setup(60, 4, 2, 100, 411, 4096);

    for (byte i = 0; i < 100; i++) {
      while (!sensor.available()) sensor.check();
      redBuffer[i] = sensor.getRed();
      irBuffer[i] = sensor.getIR();
      sensor.nextSample();
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer, 100, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }

  int obtenerBPM() {
    // mover buffers
    for (byte i = 25; i < 100; i++) {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    for (byte i = 75; i < 100; i++) {
      while (!sensor.available()) sensor.check();
      redBuffer[i] = sensor.getRed();
      irBuffer[i] = sensor.getIR();
      sensor.nextSample();
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer, 100, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    return validHeartRate ? heartRate : 0;
  }

  int obtenerSpO2() {
    return validSPO2 ? spo2 : 0;
  }
};

#endif
