#include <WiFi.h>
#include <WebServer.h>

const int ledPins[4] = {4,5,6,7};

const char* ssid = "Grande calculadora";

WebServer server(80);

const char HTML_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>16-Bit Calculator</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
        input, select, button { padding: 10px; margin: 10px; font-size: 16px; }
        #result { font-size: 20px; font-weight: bold; margin-top: 20px; }
        #overflow { color: red; font-weight: bold; margin-top: 10px; }
    </style>
</head>
<body>
    <h2>16-Bit Calculator (Two's Complement)</h2>
    <div>
        <label>Operand A:</label>
        <input type="number" id="opA" min="-32768" max="32767" value="0">

        <select id="operation">
            <option value="addition">+</option>
            <option value="subtraction">-</option>
            <option value="multiplication">*</option>
            <option value="factorial">!</option>
        </select>

        <label>Operand B:</label>
        <input type="number" id="opB" min="-32768" max="32767" value="0">
    </div>
    
    <button onclick="sendCalculation()">Calculate</button>
    
    <div id="result"></div>
    <div id="overflow"></div>

    <script>
        function sendCalculation() {
            var a = document.getElementById("opA").value;
            var b = document.getElementById("opB").value;
            var op = document.getElementById("operation").value;
            
            fetch('/calculate?op1=' + a + '&op2=' + b + '&operation=' + op)
                .then(response => response.json())
                .then(data => {
                    document.getElementById("result").innerText = "Decimal Result: " + data.decimalResult + " (Binary: " + data.binaryResult + ")";
                    if(data.overflow === 1) {
                        document.getElementById("overflow").innerText = "WARNING: Arithmetic Overflow Occurred!";
                    } else {
                        document.getElementById("overflow").innerText = "";
                    }
                });
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", HTML_INTERFACE);
}

void handleCalculation() {
  if (!server.hasArg("op1") || !server.hasArg("op2") || !server.hasArg("operation")) {
    server.send(400, "application/json", "{\"error\":\"Missing parameters\"}");
    return;
  }

  int32_t op1 = server.arg("op1").toInt();
  int32_t op2 = server.arg("op2").toInt();
  String operation = server.arg("operation");

  int32_t result = 0;
  uint8_t overflow = 0;

  if (operation == "addition") {
    result = op1 + op2;
    if (result < -32768 || result > 32767) {
      overflow = 1;
    }
  } else if (operation == "subtraction") {
    result = op1 - op2;
    if (result < -32768 || result > 32767) {
      overflow = 1;
    }
  } else if (operation == "multiplication") {
    uint32_t t_inicial = micros();
    result = op1 * op2;
    if (result < -32768 || result > 32767) {
      overflow = 1;
    }
    uint32_t t_final = micros();
    Serial.println(t_final - t_inicial);
  } else if (operation == "factorial") {
    uint32_t t_inicial = micros();
    if (op1 < 0) {
      overflow = 1;
      result = 0;
    } else {
      result = 1;
      for (int i = 1; i <= op1; i++) {
        if (result > 32767) {
          overflow = 1;
        }
        result *= i;
      }
      if (result < -32768 || result > 32767) {
        overflow = 1;
      }
    }
    uint32_t t_final = micros();
    Serial.println(t_final - t_inicial);
  }

  uint16_t value16Bits = (uint16_t)(result & 0xFFFF);

  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], (value16Bits >> i) & 0x01);
  }

  String binaryStr = "";
  for (int i = 15; i >= 0; i--) {
    binaryStr += String((value16Bits >> i) & 0x01);
  }

  String json = "{";
  json += "\"decimalResult\":" + String(result) + ",";
  json += "\"binaryResult\":\"" + binaryStr + "\",";
  json += "\"overflow\":" + String(overflow);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  WiFi.softAP(ssid);
  server.on("/", handleRoot);
  server.on("/calculate", handleCalculation);
  server.begin();
}

void loop() {
  server.handleClient();
}