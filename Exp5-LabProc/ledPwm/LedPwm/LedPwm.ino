#include <WiFi.h>
#include <WebServer.h>

const int ledPin = 4;
const int pwmChannel = 0;
const int pwmResolution = 8;

const char* ssid = "Controle LED PWM DO ANREZAO 4";

WebServer server(80);

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Controle de LED PWM</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
        input, button { padding: 10px; margin: 10px; font-size: 16px; }
        #status { font-size: 20px; font-weight: bold; margin-top: 20px; }
    </style>
</head>
<body>
    <h2>Controle de LED via PWM</h2>
    <div>
        <label>Frequência (Hz):</label>
        <input type="number" id="freq" value="5000" min="1" max="40000">
        <br>
        <label>Intensidade (0-255):</label>
        <input type="range" id="duty" min="0" max="255" value="128">
    </div>
    
    <button onclick="sendPWM()">Atualizar Parâmetros</button>
    
    <div id="status"></div>

    <script>
        function sendPWM() {
            var f = document.getElementById("freq").value;
            var d = document.getElementById("duty").value;
            
            fetch('/set_pwm?freq=' + f + '&duty=' + d)
                .then(response => response.json())
                .then(data => {
                    document.getElementById("status").innerText = "Frequência Atual: " + data.frequency + " Hz | Ciclo de Trabalho: " + data.duty;
                });
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", HTML_INTERFACE);
}

void handlePWM() {
  if (!server.hasArg("freq") || !server.hasArg("duty")) {
    server.send(400, "application/json", "{\"error\":\"Parâmetros ausentes\"}");
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
  ledcAttach(ledPin, 5000, pwmResolution);
  ledcWrite(ledPin, 128);

  WiFi.softAP(ssid);
  server.on("/", handleRoot);
  server.on("/set_pwm", handlePWM);
  server.begin();
}

void loop() {
  server.handleClient();
}