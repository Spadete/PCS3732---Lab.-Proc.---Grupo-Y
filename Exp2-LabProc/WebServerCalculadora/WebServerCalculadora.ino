
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
    <title>4-Bit Calculator</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
        select, button { padding: 10px; margin: 10px; font-size: 16px; }
        #result { font-size: 20px; font-weight: bold; margin-top: 20px; }
        #overflow { color: red; font-weight: bold; margin-top: 10px; }
    </style>
</head>
<body>
    <h2>4-Bit Calculator (Two's Complement)</h2>
    <div>
        <label>Operand A:</label>
        <select id="opA">
            <option value="7">7 (0111)</option>
            <option value="6">6 (0110)</option>
            <option value="5">5 (0101)</option>
            <option value="4">4 (0100)</option>
            <option value="3">3 (0011)</option>
            <option value="2">2 (0010)</option>
            <option value="1">1 (0001)</option>
            <option value="0">0 (0000)</option>
            <option value="-1">-1 (1111)</option>
            <option value="-2">-2 (1110)</option>
            <option value="-3">-3 (1101)</option>
            <option value="-4">-4 (1100)</option>
            <option value="-5">-5 (1011)</option>
            <option value="-6">-6 (1010)</option>
            <option value="-7">-7 (1001)</option>
            <option value="-8">-8 (1000)</option>
        </select>

        <select id="operation">
            <option value="addition">+</option>
            <option value="subtraction">-</option>
        </select>

        <label>Operand B:</label>
        <select id="opB">
            <option value="7">7 (0111)</option>
            <option value="6">6 (0110)</option>
            <option value="5">5 (0101)</option>
            <option value="4">4 (0100)</option>
            <option value="3">3 (0011)</option>
            <option value="2">2 (0010)</option>
            <option value="1">1 (0001)</option>
            <option value="0">0 (0000)</option>
            <option value="-1">-1 (1111)</option>
            <option value="-2">-2 (1110)</option>
            <option value="-3">-3 (1101)</option>
            <option value="-4">-4 (1100)</option>
            <option value="-5">-5 (1011)</option>
            <option value="-6">-6 (1010)</option>
            <option value="-7">-7 (1001)</option>
            <option value="-8">-8 (1000)</option>
        </select>
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

  int8_t op1 = (int8_t)server.arg("op1").toInt();
  int8_t op2 = (int8_t)server.arg("op2").toInt();
  String operation = server.arg("operation");

  int8_t result = 0;
  uint8_t overflow = 0;

  if (operation == "addition") {
    result = op1 + op2;
    if ((op1 > 0 && op2 > 0 && result < 0) || (op1 < 0 && op2 < 0 && result >= 0)) {
      overflow = 1;
    }
  } else if (operation == "subtraction") {
    result = op1 - op2;
    if ((op1 > 0 && op2 < 0 && result < 0) || (op1 < 0 && op2 > 0 && result >= 0)) {
      overflow = 1;
    }
  }

  uint8_t value4Bits = (uint8_t)(result & 0x0F);

  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], (value4Bits >> i) & 0x01);
  }

  String binaryStr = "";
  for (int i = 3; i >= 0; i--) {
    binaryStr += String((value4Bits >> i) & 0x01);
  }

  String json = "{";
  json += "\"decimalResult\":" + String(result) + ",";
  json += "\"binaryResult\":\"" + binaryStr + "\",";
  json += "\"overflow\":" + String(overflow);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
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
