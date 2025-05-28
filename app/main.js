const socket = new WebSocket("ws://neonguard.local:81/");
const bpmSpan = document.getElementById("bpm");
const spo2Span = document.getElementById("spo2");
const anomaliaSpan = document.getElementById("anomalia");
const nivelSpan = document.getElementById("nivel");
const estadoCalibracion = document.getElementById("estadoCalibracion");
const estadoPulsera = document.getElementById("estadoPulsera");
const btnExportarExcel = document.getElementById("btnExportarExcel");

const ctx = document.getElementById("grafica").getContext("2d");
const datos = {
  labels: [],
  datasets: [
    {
      label: "Promedio de movimientos",
      borderColor: "#3498db",
      backgroundColor: "#3498db33",
      data: [],
    },
  ],
};

const config = {
  type: "line",
  data: datos,
  options: {
    responsive: true,
    animation: false,
    scales: {
      y: { beginAtZero: true },
    },
    plugins: {
      legend: { display: false },
    },
  },
};

const grafica = new Chart(ctx, config);

function actualizarGrafica(valor) {
  const ahora = new Date().toLocaleTimeString();
  datos.labels.push(ahora);
  datos.datasets[0].data.push(valor);
  if (datos.labels.length > 10) {
    datos.labels.shift();
    datos.datasets[0].data.shift();
  }
  grafica.update();
}

function evaluarNivel(anomalia) {
  if (anomalia < 25) return ["Normal", "normal"];
  if (anomalia < 50) return ["Leve", "leve"];
  if (anomalia < 75) return ["Moderado", "moderado"];
  return ["Critico", "critico"];
}

let intervaloPeticiones;
const historico = [];

socket.onopen = () => {
  console.log("✅ Conectado al WebSocket");

  if (intervaloPeticiones) clearInterval(intervaloPeticiones);

  intervaloPeticiones = setInterval(() => {
    if (socket.readyState === WebSocket.OPEN) {
      socket.send("getEstado");
    }
  }, 200);
};

socket.onmessage = (event) => {
  try {
    const datos = JSON.parse(event.data);
    if (datos.puls !== undefined) bpmSpan.textContent = datos.puls;
    if (datos.oxigenacion !== undefined)
      spo2Span.textContent = datos.oxigenacion;
    if (datos.anomalia !== undefined) {
      anomaliaSpan.textContent = datos.anomalia;
      const [nivel, clase] = evaluarNivel(datos.anomalia);
      nivelSpan.textContent = nivel;
      nivelSpan.className = `nivel ${clase}`;
    }
    if (datos.promedio !== undefined) actualizarGrafica(datos.promedio);
    if (datos.status) estadoCalibracion.textContent = datos.status;

    if (datos.pulcera_con === true) {
      estadoPulsera.textContent = "🟢 Pulsera conectada";
      estadoPulsera.className = "conectado";
    } else {
      estadoPulsera.textContent = "🔴 Pulsera desconectada";
      estadoPulsera.className = "desconectado";
    }

    agregarAHistorico(datos);
  } catch (e) {
    console.error("❌ Error procesando datos:", e);
  }
};

function agregarAHistorico(data) {
  const timestamp = new Date().toLocaleTimeString();
  const [nivelTexto] = evaluarNivel(data.anomalia ?? 0);
  historico.push({
    tiempo: timestamp,
    bpm: data.puls ?? "",
    spo2: data.oxigenacion ?? "",
    anomalia: data.anomalia ?? "",
    promedio: data.promedio ?? "",
    nivel: nivelTexto,
  });
  if (historico.length > 25) historico.shift(); // 25 registros = 5 segundos aprox
}

btnExportarExcel.addEventListener("click", () => {
  let csv = "Tiempo,BPM,SpO2,Anomalia,Promedio,Nivel\n";
  historico.forEach((dato) => {
    csv += `${dato.tiempo},${dato.bpm},${dato.spo2},${dato.anomalia},${dato.promedio},${dato.nivel}\n`;
  });

  const blob = new Blob([csv], { type: "text/csv;charset=utf-8;" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  const ahora = new Date();
  const nombreArchivo = `historico_${ahora
    .toLocaleDateString()
    .replaceAll(
      "/",
      "-"
    )}_${ahora.getHours()}-${ahora.getMinutes()}-${ahora.getSeconds()}.csv`;
  a.download = nombreArchivo;
  a.click();
  URL.revokeObjectURL(url);
});

socket.onerror = () => {
  console.error("❌ Error con WebSocket");
  clearInterval(intervaloPeticiones);
};

socket.onclose = () => {
  console.warn("🔌 WebSocket desconectado");
  clearInterval(intervaloPeticiones);
};

document.getElementById("btnCalibrar").addEventListener("click", () => {
  if (socket.readyState === WebSocket.OPEN) {
    socket.send("calibrar");
    estadoCalibracion.textContent = "🛠 Calibrando... No ejercer presión";
  }
});
