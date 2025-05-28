# 🛡️ NeonGuard

**NeonGuard** es un sistema inteligente de monitoreo del sueño infantil que combina sensores de presión (FSR) y una pulsera biométrica para detectar signos vitales y anomalías durante el sueño. El sistema opera en tiempo real y cuenta con una interfaz web para visualizar la información y exportar reportes.

---

## 📦 Estructura del Proyecto

```
NeonGuard/
├── Pulsera/                      # Código de la pulsera (ESP32-S3)
│   ├── Pulsera.ino
│   ├── SensorPulsera.h
│   └── WiFiManagerPulsera.h

├── NeonGuardServer/             # Código del servidor (ESP32 DevKit v1)
│   ├── NeonGuardServer.ino
│   ├── ForceSensor.h
│   ├── LedController.h
│   ├── InternetAPIServer.h      # WebSocket para interfaz web
│   ├── LocalPulseraReceiver.h   # HTTP receptor desde la pulsera
│   ├── WiFiManagerServer.h
│   ├── GlobalData.h / .cpp

├── app/                         # Interfaz web
│   ├── index.html
│   ├── main.js
│   ├── styles.css
│   └── img/ (Logos)

└── README.md
```

---

## 🧠 Funcionamiento General

El sistema se compone de dos dispositivos:

### 1️⃣ Pulsera ESP32-S3

- Lee pulso y oxigenación usando sensor MAX30102
- Se conecta a `neonguard.local`
- Envía datos solo si **cambian** o si han pasado **5 segundos**
- Usa **HTTP POST** hacia `http://neonguard.local:82/pulsera`
- LED integrado se enciende cuando hay transmisión

### 2️⃣ Servidor ESP32 (DevKit1)

- Se conecta al Wi-Fi configurado
- Activa dos servidores:
  - 🛰 WebSocket (puerto 81) para interfaz web
  - 🔌 HTTP (puerto 82) para recibir datos de la pulsera
- Mide presión por sensores FSR
- Calcula nivel de anomalía cada 200 ms
- Expone todo el estado vía WebSocket

---

## 🌐 Interfaz Web (app/)

- Muestra en tiempo real:
  - 💓 BPM
  - 🩸 SpO₂
  - ⚠️ Nivel de anomalía (Normal, Leve, Moderado, Crítico)
  - 📈 Gráfica en vivo
  - 🟢 Estado de la pulsera
- Permite:
  - 🛠 Calibración remota
  - 📤 Exportar histórico a Excel (últimos 5 segundos)

---

## 📡 Modo de Comunicación

| Dispositivo      | Medio    | Protocolo | Descripción                                 |
| ---------------- | -------- | --------- | ------------------------------------------- |
| Pulsera ESP32-S3 | Cliente  | HTTP      | Envía datos en JSON cada 5s o si hay cambio |
| Servidor ESP32   | Servidor | WebSocket | Transmite datos a interfaz web (dashboard)  |
| Interfaz Web     | Cliente  | WebSocket | Consulta cada 200 ms vía `getEstado`        |

---

## 🧪 Lógica de Anomalía

### Parámetros considerados:

| Criterio               | Condición                      | % Riesgo |
| ---------------------- | ------------------------------ | -------- |
| Movimiento brusco      | Δ presión > 180                | +15%     |
| Inactividad prolongada | sin cambio > 5 ciclos (10s)    | +20%     |
| Distribución anormal   | solo 1 sensor activo o todos 0 | +10–25%  |
| BPM anormal            | < 50 o > 180                   | +25%     |
| Oxigenación baja       | SpO₂ < 92%                     | +30%     |

### Niveles interpretados

| Porcentaje | Nivel de Riesgo | Descripción                                 |
| ---------- | --------------- | ------------------------------------------- |
| 0–25%      | 🟢 Normal       | Variaciones normales del sueño              |
| 26–50%     | 🟡 Leve         | Incomodidad o inicio de irregularidad       |
| 51–75%     | 🟠 Moderado     | Posible apnea leve o postura incorrecta     |
| 76–100%    | 🔴 Crítico      | Apnea severa, paro respiratorio, caída, etc |

---

## 🛠 Calibración de Sábana Sensorial

1. Asegúrate de que la sábana esté libre y sin presión
2. Accede a la interfaz web
3. Haz clic en "🛠 Calibrar"
4. Espera a que el LED parpadee lentamente (🟦)
5. Aparecerá el mensaje `✅ Calibración completada.`
6. Coloca al infante y comienza el monitoreo

---

## 🧰 Requisitos Técnicos

- 🔌 ESP32 DevKit v1 (Servidor)
- 📲 ESP32-S3 (Pulsera)
- ✅ Arduino IDE 2.x
- Librerías necesarias:
  - `WiFiManager`
  - `WebSocketsServer`
  - `ArduinoJson`
  - `Preferences`
  - `HttpClient`
  - `WebServer`

---

## 🚀 Uso

1. Conecta ambos ESP32 vía USB
2. Sube `Pulsera.ino` al ESP32-S3 y `NeonGuardServer.ino` al DevKit1
3. Abre `app/index.html` en un navegador conectado a la misma red Wi-Fi
4. Observa los datos en tiempo real

---

## 🧽 Reiniciar Configuración Wi-Fi

- En el monitor serial escribe `-1`
- O mantén presionado el botón por 5 segundos

---

## 👤 Desarrolladores

Desarrollado por el equipo de **NeonGuard**  
Instituto Tecnológico Superior de Monclova

### 👨‍💻 Programador

- [Alexander Uriel Torres Pérez](https://www.facebook.com/share/1BvaWjtfng/)

### 🎨 Diseñador y modelador

- [Luis Ángel Córdova Ceniceros](https://www.facebook.com/luis.cordova.79462815)

### 🔌 Eléctrico

- [Pablo Emiliano Sánchez Campos](https://www.facebook.com/pablo.sanchez.251275)

### 👩‍💼 Administradoras

- [Aranza Dalay Vidrio Vásquez](https://www.facebook.com/share/16NrDxJBV5/)
- [Xitlalic Jacqueline García Saldaña](https://www.facebook.com/share/1HsM3pJrV9/)
