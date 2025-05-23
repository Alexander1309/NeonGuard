#include <Preferences.h>

class ForceSensor {
private:
  const uint8_t sensorPins[4] = { 34, 35, 32, 33 };
  int sensorValues[4];
  int previousValues[4];
  int offsets[4];
  unsigned long lastMovementTime;
  const int movementThreshold = 180;
  const int inactivityTime = 60000;
  const int minValue = 30;
  const int maxInactivityCycles = 5;
  int inactivityCounter = 0;
  Preferences preferences;

public:
  // 🔹 Inicialización: usa valores base de offset
  void begin() {
    for (int i = 0; i < 4; i++) {
      pinMode(sensorPins[i], INPUT);
      sensorValues[i] = 0;
      previousValues[i] = 0;
      offsets[i] = 200;  // Offset estimado para sensores sin carga
    }

    Serial.println("📦 Usando valores de offset predeterminados.");
    Serial.println("📌 Realiza calibración cuando el bebé esté en posición.");

    lastMovementTime = millis();
  }

  // 1️⃣ Predicción de anomalía (%)
  int predictAnomaly(int bpm, int spo2) {
    readSensors();
    int anomalyScore = 0;

    // 1. Movimiento brusco
    for (int i = 0; i < 4; i++) {
      int diff = abs(sensorValues[i] - previousValues[i]);
      if (diff > movementThreshold) anomalyScore += 15;
    }

    // 2. Inactividad prolongada
    bool stable = true;
    for (int i = 0; i < 4; i++) {
      if (sensorValues[i] != previousValues[i]) stable = false;
    }

    if (stable) {
      inactivityCounter++;
      if (inactivityCounter >= maxInactivityCycles) anomalyScore += 20;
    } else {
      inactivityCounter = 0;
    }

    // 3. Baja presión o desbalance
    int active = 0;
    for (int i = 0; i < 4; i++) {
      if (sensorValues[i] > minValue) active++;
    }

    if (active == 0) anomalyScore += 25;
    else if (active == 1) anomalyScore += 10;

    // 4. Pulsaciones anormales
    if (bpm < 50 || bpm > 180) {
      Serial.println("⚠️ Ritmo cardíaco anormal detectado");
      anomalyScore += 25;
    }

    // 5. Oxigenación baja
    if (spo2 < 92) {
      Serial.println("⚠️ Oxigenación baja detectada");
      anomalyScore += 30;
    }

    return constrain(anomalyScore, 0, 100);
  }


  // 2️⃣ Calibración y guardado en flash
  void calibrate(int samples = 50, int delayMs = 20) {
    Serial.println("🛠️ Calibrando... No aplicar presión.");
    int sum[4] = { 0, 0, 0, 0 };

    for (int s = 0; s < samples; s++) {
      for (int i = 0; i < 4; i++) {
        sum[i] += analogRead(sensorPins[i]);
      }
      delay(delayMs);
    }

    preferences.begin("ForceData", false);
    for (int i = 0; i < 4; i++) {
      offsets[i] = sum[i] / samples;
      String key = "offset" + String(i);
      preferences.putInt(key.c_str(), offsets[i]);
    }
    preferences.putBool("calibrated", true);
    preferences.end();

    Serial.println("✅ Calibración completada y guardada.");
    for (int i = 0; i < 4; i++) {
      Serial.print("S");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(offsets[i]);
      Serial.print("  ");
    }
    Serial.println();
  }

  // 3️⃣ Imprime valores por sensor
  void printSensorValues() {
    readSensors();
    Serial.print("Sensores [");
    for (int i = 0; i < 4; i++) {
      Serial.print(sensorValues[i]);
      if (i < 3) Serial.print(", ");
    }
    Serial.println("]");
  }

  // 4️⃣ Retorna promedio de los 4 sensores
  int getAverageSignal() {
    readSensors();
    int sum = 0;
    for (int i = 0; i < 4; i++) {
      sum += sensorValues[i];
    }
    return sum / 4;
  }

private:
  // 🔹 Lee sensores con offset aplicado
  void readSensors() {
    for (int i = 0; i < 4; i++) {
      previousValues[i] = sensorValues[i];
      sensorValues[i] = analogRead(sensorPins[i]) - offsets[i];
      if (sensorValues[i] < 0) sensorValues[i] = 0;
    }
  }
};
