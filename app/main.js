const socket = new WebSocket("ws://neonguard.local:81/");
const bpmSpan = document.getElementById("bpm");
const spo2Span = document.getElementById("spo2");
const anomaliaSpan = document.getElementById("anomalia");
const nivelSpan = document.getElementById("nivel");
const estadoCalibracion = document.getElementById("estadoCalibracion");

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
    scales: {
      y: { beginAtZero: true },
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
  return ["Crítico", "critico"];
}

socket.onopen = () => {
  console.log("✅ Conectado al WebSocket");
  setInterval(() => socket.send("getEstado"), 2000);
};

socket.onmessage = (event) => {
  const datos = JSON.parse(event.data);
  if (datos.puls !== undefined) bpmSpan.textContent = datos.puls;
  if (datos.oxigenacion !== undefined) spo2Span.textContent = datos.oxigenacion;
  if (datos.anomalia !== undefined) {
    anomaliaSpan.textContent = datos.anomalia;
    const [nivel, clase] = evaluarNivel(datos.anomalia);
    nivelSpan.textContent = nivel;
    nivelSpan.className = `nivel ${clase}`;
  }
  if (datos.promedio !== undefined) actualizarGrafica(datos.promedio);
  if (datos.status) estadoCalibracion.textContent = datos.status;
};

socket.onerror = () => {
  console.error("❌ Error con WebSocket");
};

document.getElementById("btnCalibrar").addEventListener("click", () => {
  socket.send("calibrar");
  estadoCalibracion.textContent = "🛠 Calibrando No Ejercer Preción...";
});
