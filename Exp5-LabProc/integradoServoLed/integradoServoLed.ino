#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const int ledPin = 4;
const int pwmResolution = 8;
const int servoPin = 5;

const char* ssid = "Controle LED e Servo Andrezao";

WebServer server(80);
Servo myServo;

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Controle Integrado ESP32</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 30px; background-color: #f4f4f9; }
        h2 { color: #333; }
        .section { border: 2px solid #ccc; padding: 20px; margin: 20px auto; width: 80%; max-width: 500px; border-radius: 10px; background-color: #fff; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        h3 { margin-top: 0; color: #444; border-bottom: 1px solid #eee; padding-bottom: 10px; }
        input, button { padding: 10px; margin: 10px; font-size: 16px; }
        input[type="number"] { width: 100px; text-align: center; }
        input[type="range"] { width: 80%; }
        .status { font-size: 18px; font-weight: bold; margin-top: 15px; color: #0066cc; min-height: 24px; }
    </style>
</head>
<body>
    <h2>Painel de Controle Integrado</h2>
    
    <div class="section">
        <h3>Controle do Servomotor</h3>
        <div>
            <label>Ângulo (0 a 180 graus):</label>
            <input type="range" id="angle" min="0" max="180" value="90" oninput="updateLabel(this.value)">
            <span id="angleLabel">90</span>&deg;
        </div>
        <button onclick="sendAngle()">Mover Servo</button>
        <div id="statusServo" class="status"></div>
    </div>

    <div class="section">
        <h3>Controle de LED via PWM</h3>
        <div>
            <label>Frequência (Hz):</label>
            <input type="number" id="freq" value="5000" min="1" max="40000">
            <br>
            <label>Intensidade (0-255):</label>
            <input type="range" id="duty" min="0" max="255" value="128">
        </div>
        <button onclick="sendPWM()">Atualizar Parâmetros</button>
        <div id="statusLED" class="status"></div>
    </div>

    <script>
        function updateLabel(val) {
            document.getElementById("angleLabel").innerText = val;
        }
        
        function sendAngle() {
            var a = document.getElementById("angle").value;
            fetch('/set_servo?angle=' + a)
                .then(response => response.json())
                .then(data => {
                    document.getElementById("statusServo").innerText = "Ângulo Definido: " + data.angle + " graus";
                });
        }

        function sendPWM() {
            var f = document.getElementById("freq").value;
            var d = document.getElementById("duty").value;
            fetch('/set_pwm?freq=' + f + '&duty=' + d)
                .then(response => response.json())
                .then(data => {
                    document.getElementById("statusLED").innerText = "Frequência: " + data.frequency + " Hz | Intensidade: " + data.duty;
                });
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", HTML_INTERFACE);
}

void handleServo() {
  if (!server.hasArg("angle")) {
    server.send(400, "application/json", "{\"error\":\"Angulo ausente\"}");
    return;
  }

  int angle = server.arg("angle").toInt();
  myServo.write(angle);

  String json = "{";
  json += "\"angle\":" + String(angle);
  json += "}";

  server.send(200, "application/json", json);
}

void handlePWM() {
  if (!server.hasArg("freq") || !server.hasArg("duty")) {
    server.send(400, "application/json", "{\"error\":\"Parametros ausentes\"}");
    return;
  }

  int frequency = server.arg("freq").toInt();
  int duty = server.arg("duty").toInt();

  ledcAttach(ledPin, frequency, pwmResolution);
  ledcWrite(ledPin, duty);

  String json = "{";
  json += "\"frequency\":" + String(frequency) + ",";
  json += "\"duty\":" + String(duty);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  myServo.attach(servoPin);
  myServo.write(90);

  ledcAttach(ledPin, 5000, pwmResolution);
  ledcWrite(ledPin, 128);

  WiFi.softAP(ssid);
  server.on("/", handleRoot);
  server.on("/set_servo", handleServo);
  server.on("/set_pwm", handlePWM);
  server.begin();
}

void loop() {
  server.handleClient();
}