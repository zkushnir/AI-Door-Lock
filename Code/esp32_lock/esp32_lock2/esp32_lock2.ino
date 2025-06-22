#include <WiFi.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "Thebasement";
const char* password = "Fmab1357";

// Servo and transistor pins
const int servoPin = 13;           // Servo control pin
const int servoPowerPin = 12;      // Transistor base pin to control servo power

// Button pin
const int buttonPin = 14;          // Manual lock/unlock button

// OLED display config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 4
#define OLED_SCL 15

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WebServer server(5001); // HTTP server on port 5001

Servo doorServo;

bool doorLocked = true;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Servo positions
const int lockedPos = 180;    // Adjust for your servo locked position
const int unlockedPos = 0; // Adjust for your servo unlocked position

// Unlock duration
const unsigned long unlockDuration = 10000; // milliseconds

unsigned long unlockStartTime = 0;
bool unlocking = false;

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  if (doorLocked) {
    display.println("Locked");
  } else {
    display.println("Unlocked");
  }
  display.display();
}

void powerServo(bool on) {
  digitalWrite(servoPowerPin, on ? HIGH : LOW);
  delay(20); // Give time for power to stabilize
}

void lockDoor() {
  powerServo(true);
  doorServo.write(lockedPos);
  delay(1000); // Give servo time to move
  powerServo(false);
  doorLocked = true;
  updateDisplay();
}

void unlockDoor() {
  powerServo(true);
  doorServo.write(unlockedPos);
  delay(1000);
  powerServo(false);
  doorLocked = false;
  updateDisplay();
  unlocking = true;
  unlockStartTime = millis();
}

void handleUnlockRequest() {
  //if (doorLocked) {
    unlockDoor();
    Serial.println("Door unlocked via network");
  //}
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  pinMode(servoPowerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Wire.begin(OLED_SDA, OLED_SCL);
  delay(100);  // Give time for I2C to settle
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Starting...");
  display.display();

  doorServo.attach(servoPin);

  // Lock door at startup
  lockDoor();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("IP:");
  display.println(WiFi.localIP());
  display.display();
  delay(500);
  // Setup web server route
  server.on("/unlock", HTTP_GET, handleUnlockRequest);
  server.begin();
}

void loop() {
  server.handleClient();

  // Check if door is currently unlocked and if unlockDuration passed
  if (unlocking && millis() - unlockStartTime >= unlockDuration) {
    lockDoor();
    unlocking = false;
    Serial.println("Door locked automatically");
  }

  // Manual button reading with debounce
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW) { // Button pressed (active low)
      if (doorLocked) {
        unlockDoor();
        Serial.println("Door unlocked manually");
      } else {
        lockDoor();
        Serial.println("Door locked manually");
      }
      delay(300); // Simple debounce delay to avoid rapid toggling
    }
  }
  lastButtonState = reading;
}
