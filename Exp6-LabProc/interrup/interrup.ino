#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "Andrezao_LDR";

const int ldrPin = 0; 
const int ledPin = 8;
const int buttonPin = 4;
const int numPixels = 1;
const int lowLightThreshold = 3000;

WebServer server(80);
Adafruit_NeoPixel pixels(numPixels, ledPin, NEO_GRB + NEO_KHZ800);

int ldrValue = 0;
unsigned long lastReadTime = 0;
unsigned long lastBlinkTime = 0;
bool ledState = false;

volatile bool sosTriggered = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

bool sosActive = false;
unsigned long sosStartTime = 0;

void IRAM_ATTR buttonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    sosTriggered = true;
    lastDebounceTime = currentTime;
  }
}

void handleRoot() {
  String statusClass = (ldrValue > lowLightThreshold) ? "dark" : "light";
  String statusText = (ldrValue > lowLightThreshold) ? "Ambiente Escuro" : "Ambiente Claro";

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "<title>Painel LDR</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f6f9; color: #333; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  html += ".card { background: white; padding: 30px; border-radius: 12px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); text-align: center; width: 300px; }";
  html += "h1 { font-size: 20px; color: #2c3e50; margin-bottom: 20px; }";
  html += ".value { font-size: 48px; font-weight: bold; color: #007bff; margin: 20px 0; }";
  html += ".status { display: inline-block; padding: 8px 16px; border-radius: 20px; font-size: 14px; font-weight: bold; }";
  html += ".dark { background-color: #ffe6e6; color: #dc3545; }";
  html += ".light { background-color: #e6f4ff; color: #0056b3; }";
  html += "</style></head><body>";
  html += "<div class='card'>";
  html += "<h1>Luminosidade</h1>";
  html += "<div class='value'>" + String(ldrValue) + "</div>";
  html += "<div class='status " + statusClass + "'>" + statusText + "</div>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.clear();
  pixels.show();

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);

  WiFi.softAP(ssid);

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= 1000) {
    ldrValue = analogRead(ldrPin);
    lastReadTime = currentTime;
  }

  if (sosTriggered) {
    Serial.println("Botao SOS pressionado!");
    sosTriggered = false;
    sosActive = true;
    sosStartTime = currentTime;
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
  }

  if (sosActive) {
    if (currentTime - sosStartTime >= 3000) {
      sosActive = false;
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.show();
    }
  } else {
    if (ldrValue > lowLightThreshold) {
      if (currentTime - lastBlinkTime >= 2000) {
        ledState = !ledState;
        if (ledState) {
          pixels.setPixelColor(0, pixels.Color(255, 255, 0));
        } else {
          pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        }
        pixels.show();
        lastBlinkTime = currentTime;
      }
    } else {
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.show();
    }
  }
}