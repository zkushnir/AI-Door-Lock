#include <ESP32Servo.h>

const int buttonPin = 12;
const int servoPin = 13;
const int powerPin = 15;  // Transistor control

Servo myServo;
bool lastButtonState = HIGH;
bool servoOpen = false;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);  // Start with power OFF

  Serial.begin(115200);
  Serial.println("Ready");
}

void activateServo(int angle) {
  digitalWrite(powerPin, HIGH); // Power ON
  delay(100); // Let servo stabilize
  myServo.attach(servoPin);
  myServo.write(angle);
  delay(700); // Wait for move
  myServo.detach();
  digitalWrite(powerPin, LOW); // Power OFF
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    servoOpen = !servoOpen;

    if (servoOpen) {
      activateServo(110); // Open
      Serial.println("Unlocked");
    } else {
      activateServo(0);  // Closed
      Serial.println("Locked");
    }

    delay(500); // Debounce
  }

  lastButtonState = currentButtonState;
}
