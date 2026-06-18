const int buttonPin = 4;
const int externalLedPin = 5;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(externalLedPin, OUTPUT);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {
    digitalWrite(externalLedPin, HIGH);
  } else {
    digitalWrite(externalLedPin, LOW);
  }
}