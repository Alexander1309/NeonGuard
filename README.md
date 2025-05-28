# 🛡️ NeonGuard

**NeonGuard** es un sistema inteligente de monitoreo del sueño infantil que opera en dos modos:

- 🌐 **Modo Internet:** Si hay Wi-Fi, los datos de la pulsera se consultan desde un servidor HTTP y WebSocket.
- 📶 **Modo Local (AP):** Si no hay internet, el sistema activa un Access Point local que espera datos directamente por WebSocket.

Este sistema permite registrar y visualizar en tiempo real los signos vitales de un infante, como el pulso, movimiento y nivel de oxigenación.

---

## 📦 Estructura del Proyecto

```
NeonGuard/
├── NeonGuard.ino                 # Archivo principal, coordinación de tareas y sensores
├── ForceSensor.h                 # Lectura de sensores FSR, análisis de sueño y anomalías
├── WiFiManagerServer.h           # Manejo de conexión Wi-Fi y credenciales almacenadas
├── InternetAPIServer.h           # Servidor WebSocket en modo Internet
├── LocalPulseraReceiver.h        # Servidor WebSocket receptor de datos de la pulsera (modo AP)
├── LedController.h               # Control del LED RGB según estado del sistema
├── GlobalData.h / GlobalData.cpp # Variables compartidas entre módulos y configuración global
```

---

## 🧠 Características

- ✅ Auto-conexión a redes Wi-Fi guardadas (WiFiManager)
- 🔁 Cambio automático a red local si no hay internet
- 💾 Borrado de credenciales manteniendo presionado un botón por 5 segundos
- 📡 WebSocket bidireccional en modo Internet (consulta de estado y calibración)
- 📲 WebSocket receptor dedicado para datos en modo AP (desde la pulsera)
- 🛠️ Calibración remota desde la interfaz web con botón y feedback visual

---

## 🖼️ Logos

### Instituto Tecnológico Superior de Monclova

![Logo Tec](./img/LogoTecNm.svg)

### Proyecto NeonGuard

![Logo NeonGuard](./img/neonguard.svg)

---

## ⚙️ Requisitos

- ESP32 DevKit v1
- Arduino IDE 2.x o VSCode + PlatformIO
- Librerías necesarias:
  - `WiFiManager`
  - `WebSocketsServer`
  - `Preferences`
  - `ArduinoJson`

---

## 🔧 Cómo usar

1. Sube el sketch `NeonGuard.ino` a tu ESP32.
2. Si no hay Wi-Fi configurado, se crea una red llamada `NeonGuard_Config`.
3. Ingresa desde tu celular o PC, y configura el Wi-Fi.
4. Si el sistema no detecta internet después de 60 segundos, cambia automáticamente a modo local (Access Point).
5. Para borrar las credenciales Wi-Fi, **mantén presionado el botón de reset durante 5 segundos**.

---

## 🎯 Calibración de la sábana inteligente

### 🧼 Paso a paso para calibrar correctamente:

1. **Preparación:** Asegúrate de que la sábana esté completamente extendida, limpia y sin ningún objeto ni presión.
2. **Encendido:** Conecta el ESP32 y espera a que se muestre la IP local en el monitor serie, por ejemplo: `📡 IP local: 192.168.1.108`
3. **Accede al navegador:** En un dispositivo conectado a la misma red, entra a la interfaz web.
4. **Haz clic en el botón "🛠️ Calibrar"**. El LED comenzará a parpadear lentamente 🟦 indicando que la calibración está en proceso.
5. **Confirmación:** En pantalla aparecerá el mensaje: `✅ Calibración completada.`
6. **Listo:** La sábana estará lista para usarse. Puedes ahora colocar al bebé y comenzar el monitoreo.

---

## 📡 API - Modo Internet

- `GET /estado` o por WebSocket
  Devuelve un JSON con el estado actual de la pulsera:

```json
{
  "puls": 78,
  "oxigenacion": 98,
  "promedio": 40,
  "anomalia": 45
}
```

---

### 📊 Tabla de Predicción de Anomalías en NeonGuard

| 💡 Criterio Evaluado                     | Condición Detectada                        | % Asignado | Explicación Técnica                                                    |
| ---------------------------------------- | ------------------------------------------ | ---------- | ---------------------------------------------------------------------- |
| 🔺 **Movimiento brusco** (FSR)           | Cambio súbito de presión > 180             | +15%       | Indicador de espasmos, sobresaltos, caídas o sueño agitado             |
| 💤 **Inactividad prolongada** (FSR)      | Sin cambios de presión por 5 ciclos (~10s) | +20%       | Posible apnea, muerte de cuna o pérdida de conciencia                  |
| ⚖️ **Distribución anormal** (FSR)        | Solo 1 sensor activo o todos en 0          | +10–25%    | Postura errática, abandono de cama o desconexión de sensores           |
| ❤️ **Frecuencia cardíaca anormal** (BPM) | BPM < 50 o BPM > 180                       | +25%       | Bradicardia o taquicardia, riesgos de paro o crisis cardíaca           |
| 🫁 **Oxigenación baja** (SpO₂)           | SpO₂ < 92%                                 | +30%       | Hipoxemia crítica; puede preceder eventos de apnea o paro respiratorio |

### 🔢 Ejemplo de cálculo de anomalía

| Parámetro             | Valor Detectado     | ¿Cumple Condición? | % Asignado |
| --------------------- | ------------------- | ------------------ | ---------- |
| Movimiento brusco     | Sensor 2 cambió 210 | ✅                 | 15%        |
| Inactividad           | 5 ciclos sin cambio | ✅                 | 20%        |
| Presión desbalanceada | Solo sensor 3 > 30  | ✅                 | 10%        |
| BPM                   | 43 BPM              | ✅                 | 25%        |
| SpO₂                  | 91%                 | ✅                 | 30%        |
| **Total estimado**    |                     |                    | **100%**   |

### 📘 Interpretación de puntuaciones

| Porcentaje (%) | Nivel de riesgo | Posibles causas                                                    |
| -------------- | --------------- | ------------------------------------------------------------------ |
| 0–25%          | 🟢 Normal       | Variaciones naturales del sueño                                    |
| 26–50%         | 🟡 Leve         | Movimiento leve o inicio de irregularidad                          |
| 51–75%         | 🟠 Moderado     | Apnea leve, postura anómala, oxigenación baja moderada             |
| 76–100%        | 🔴 Crítico      | Paro respiratorio, apnea severa, muerte súbita, caída, desconexión |

---

## 🧪 Borrar credenciales manualmente

- Puedes enviar `-1` por el monitor serial para reiniciar la configuración Wi-Fi.
- O mantener presionado el botón de reinicio por **5 segundos** para restaurar ajustes de red.

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
