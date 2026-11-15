#include <WiFi.h>
#include <WebServer.h>

// ---------- RETO 1: identidad propia y GPIO18 ----------
const char* NOMBRE_RED = "ESP32_LED_G01";      // Cambia G01 por tu numero de grupo
const char* CLAVE_RED  = "Mecatronica2026";
const int PIN_LED = 18;                        // El circuito tambien debe ir a GPIO18

// ---------- RETO 3: autoapagado ----------
const unsigned long TIEMPO_AUTOAPAGADO = 10000; // 10 s en milisegundos
unsigned long marcaEncendido = 0;               // Marca temporal del ultimo encendido

WebServer servidor(80);
bool ledEncendido = false;

void mostrarPagina() {
  String pagina = R"HTML(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="3">
  <title>Grupo 01 - Control LED ESP32</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin: 40px; background: #f5f7fb; }
    button { font-size: 20px; padding: 15px 25px; margin: 8px; cursor: pointer; }
  </style>
</head>
<body>
  <h1>Grupo 01: Control de LED por Wi-Fi</h1>
  <p>Estado actual: <strong>%ESTADO%</strong></p>
  <form action="/alternar" method="POST">
    <button type="submit">Cambiar estado</button>
  </form>
  <form action="/encender" method="POST">
    <button type="submit">Encender (10 s)</button>
  </form>
  <form action="/apagar" method="POST">
    <button type="submit">Apagar</button>
  </form>
</body>
</html>
)HTML";

  pagina.replace("%ESTADO%", ledEncendido ? "ENCENDIDO" : "APAGADO");
  servidor.send(200, "text/html; charset=utf-8", pagina);
}

// Funcion unica que aplica el estado al hardware y a la variable.
void fijarLED(bool encender) {
  ledEncendido = encender;
  digitalWrite(PIN_LED, encender ? HIGH : LOW);
  if (encender) {
    marcaEncendido = millis();   // Reinicia el plazo de 10 s (Reto 3c)
  }
}

void redirigirInicio() {
  servidor.sendHeader("Location", "/");
  servidor.send(303, "text/plain", "");
}

void encenderLED() {
  fijarLED(true);
  Serial.println("Orden recibida: encender LED");
  redirigirInicio();
}

void apagarLED() {
  fijarLED(false);
  Serial.println("Orden recibida: apagar LED");
  redirigirInicio();
}

// ---------- RETO 2: un solo boton para alternar ----------
void alternarLED() {
  fijarLED(!ledEncendido);
  Serial.println(ledEncendido ? "Alternar: ahora ENCENDIDO" : "Alternar: ahora APAGADO");
  redirigirInicio();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  WiFi.mode(WIFI_AP);
  bool redCreada = WiFi.softAP(NOMBRE_RED, CLAVE_RED);

  if (!redCreada) {
    Serial.println("ERROR: no se pudo crear la red Wi-Fi");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("Red Wi-Fi creada correctamente");
  Serial.print("Nombre: ");
  Serial.println(NOMBRE_RED);
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.softAPIP());

  servidor.on("/", HTTP_GET, mostrarPagina);
  servidor.on("/encender", HTTP_POST, encenderLED);
  servidor.on("/apagar", HTTP_POST, apagarLED);
  servidor.on("/alternar", HTTP_POST, alternarLED);   // Reto 2
  servidor.begin();
  Serial.println("Servidor HTTP listo");
}

void loop() {
  servidor.handleClient();

  // RETO 3: comprobacion NO bloqueante del autoapagado.
  // La resta con unsigned long funciona bien aunque millis() desborde.
  if (ledEncendido && (millis() - marcaEncendido >= TIEMPO_AUTOAPAGADO)) {
    fijarLED(false);
    Serial.println("Autoapagado: pasaron 10 s");
  }
}
