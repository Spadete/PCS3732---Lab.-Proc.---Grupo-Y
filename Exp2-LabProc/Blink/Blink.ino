#include <Adafruit_NeoPixel.h>

// Configurações do Hardware
#define PIN        8   // Pino do LED RGB no YD-ESP32-C3
#define NUMPIXELS  1   // Quantidade de LEDs integrados

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
 pixels.begin(); 
}


void loop() {
  // --- VERDE ---
  // R: 0, G: 255, B: 0
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show();
  delay(3000);

  // --- VERMELHO ---
  // R: 255, G: 0, B: 0
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();
  delay(4000);

  // --- AMARELO ---
  // R: 255, G: 255, B: 0
  pixels.setPixelColor(0, pixels.Color(255, 255, 0));
  pixels.show();
  delay(1000);
}