#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const int servoPin = 5;

const char* ssid = "Controle Servo Web Andrezao 5";

WebServer server(80);
Servo myServo;

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Controle do Servomotor</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
        input, button { padding: 10px; margin: 10px; font-size: 16px; }
        #status { font-size: 20px; font-weight: bold; margin-top: 20px; }
    </style>
</head>
<body>
    <h2>Controle de Posicao do Servomotor</h2>
    <div>
        <label>Angulo (0 a 180 graus):</label>
        <input type="range" id="angle" min="0" max="180" value="90" oninput="updateLabel(this.value)">
        <span id="angleLabel">90</span>&deg;
    </div>
    
    <button onclick="sendAngle()">Mover Servo</button>
    
    <div id="status"></div>

    <script>
        function updateLabel(val) {
            document.getElementById("angleLabel").innerText = val;
        }
        
        function sendAngle() {
            var a = document.getElementById("angle").value;
            
            fetch('/set_servo?angle=' + a)
                .then(response => response.json())
                .then(data => {
                    document.getElementById("status").innerText = "Angulo Definido: " + data.angle + " graus";
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

void setup() {
  myServo.attach(servoPin);
  myServo.write(90);

  WiFi.softAP(ssid);
  server.on("/", handleRoot);
  server.on("/set_servo", handleServo);
  server.begin();
}

void loop() {
  server.handleClient();
}