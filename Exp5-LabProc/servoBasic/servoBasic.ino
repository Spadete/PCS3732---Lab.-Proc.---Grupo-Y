#include <ESP32Servo.h>

Servo myServo;
const int servoPin = 5;

void setup() {
  myServo.attach(servoPin);
}

void loop() {
  myServo.write(0);
  delay(1000);
  
  myServo.write(90);
  delay(1000);
  
  myServo.write(180);
  delay(1000);
}