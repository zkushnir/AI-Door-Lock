#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Thebasement2";
const char* password = "Fmab13572";

// Replace these with your 2.4GHz access point BSSID and channel
uint8_t bssid[] = {0xBA, 0x5E, 0x71, 0x55, 0xCE, 0xCA};
// Channel (must match your 2.4 GHz access point)
int channel = 5;

// Optional static IP config (replace with your network details if desired)
IPAddress local_IP(10, 0, 0, 100);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// Server URL for image upload
const char* serverUrl = "http://10.0.0.195:5000/upload";

// Pin definitions
const int buttonPin = 12;
const int flashPin = 4;
const int oledPowerPin = 15;
#define OLED_SDA 14
#define OLED_SCL 2

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool oledOn = false;
unsigned long oledOnTime = 0;

void displayMessage(const char* message) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println(message);
  display.display();
}

void drawIdle() {
  display.clearDisplay();
  display.setCursor(10, 20);
  display.setTextSize(2);
  display.println("Ready");
  display.display();
}

void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    displayMessage("Cam Error");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(flashPin, OUTPUT);
  pinMode(oledPowerPin, OUTPUT);

  esp_sleep_enable_ext0_wakeup((gpio_num_t)buttonPin, 0);
  if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Not wakeup. Going to sleep.");
    esp_deep_sleep_start();
  }

  digitalWrite(oledPowerPin, HIGH);
  oledOnTime = millis();
  oledOn = true;

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED init failed"));
    while (1);
  }

  displayMessage("Hold...");
  Serial.println("Connecting to WiFi...");

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password, channel, bssid);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 8000;
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    delay(300);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    displayMessage("WiFi fail");
    Serial.println("\nWiFi failed. Sleeping...");
    delay(2000);
    display.clearDisplay(); display.display();
    digitalWrite(oledPowerPin, LOW);
    esp_deep_sleep_start();
  }

  startCamera();
  drawIdle();
}

void loop() {
  if (oledOn && (millis() - oledOnTime > 60000)) {
    display.clearDisplay();
    display.display();
    digitalWrite(oledPowerPin, LOW);
    oledOn = false;
    Serial.println("OLED off.");
  }

  if (!oledOn) {
    digitalWrite(oledPowerPin, HIGH);
    oledOnTime = millis();
    oledOn = true;
  }

  displayMessage("Capturing...");
  Serial.println("Capturing image...");

  for (int i = 0; i < 3; i++) {
    camera_fb_t* temp = esp_camera_fb_get();
    if (temp) esp_camera_fb_return(temp);
    delay(100);
  }

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    displayMessage("Capture fail");
    Serial.println("Capture failed");
    delay(2000);
    display.clearDisplay(); display.display();
    digitalWrite(oledPowerPin, LOW);
    esp_deep_sleep_start();
  }

  displayMessage("Uploading...");
  Serial.println("Uploading...");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "image/jpeg");

    int httpResponseCode = http.POST(fb->buf, fb->len);
    String payload;

    if (httpResponseCode > 0) {
      payload = http.getString();
      StaticJsonDocument<200> doc;
      if (deserializeJson(doc, payload)) {
        displayMessage("JSON Error");
        Serial.println("JSON error");
      } else {
        const char* result = doc["result"];
        if (result) {
          displayMessage(result);
          Serial.printf("Result: %s\n", result);
        } else {
          displayMessage("No Result");
        }
      }
    } else {
      displayMessage("Upload fail");
      Serial.printf("POST failed: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    displayMessage("WiFi Error");
    Serial.println("WiFi error");
  }

  esp_camera_fb_return(fb);
  delay(2000);

  display.clearDisplay();
  display.display();
  digitalWrite(oledPowerPin, LOW);
  oledOn = false;

  Serial.println("Sleeping...");
  esp_deep_sleep_start();
}
