#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// WiFi credentials
const char* ssid = "Thebasement";
const char* password = "Fmab1357";

// Server endpoint
const char* serverUrl = "http://10.0.0.136:5000/upload";

// GPIO for the button and flash
const int buttonPin = 12;
const int flashPin = 4;  // GPIO 4 controls the flash (LED)

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 14
#define OLED_SCL 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool lastButtonState = LOW;

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

  if(psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    displayMessage("Camera init failed");
    return;
  }
}

void displayMessage(const char* message) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println(message);
  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(flashPin, OUTPUT);

  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while(1);
  }

  displayMessage("Starting WiFi...");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  displayMessage("WiFi Connected");

  startCamera();
  displayMessage("Ready");
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin) == LOW;

  if (currentButtonState && !lastButtonState) {
    Serial.println("Button pressed. Flushing old frames...");
    displayMessage("Capturing...");

    digitalWrite(flashPin, HIGH);
    delay(100);

    for (int i = 0; i < 3; i++) {
      camera_fb_t* temp = esp_camera_fb_get();
      if (temp) esp_camera_fb_return(temp);
      delay(100);
    }

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      displayMessage("Capture failed");
      digitalWrite(flashPin, LOW);
      return;
    }

    Serial.println("Photo captured. Uploading...");
    displayMessage("Uploading...");

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "image/jpeg");

      int httpResponseCode = http.POST(fb->buf, fb->len);
      String response = http.getString();  // Get server's face recognition result

      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      if (httpResponseCode == 200) {
        display.println("Upload Success:");
        display.println(response);  // Show recognition result
      } else {
        display.println("Upload Fail");
        display.print("Code: ");
        display.println(httpResponseCode);
      }

      display.display();
      http.end();
    } else {
      Serial.println("WiFi not connected");
      displayMessage("WiFi Error");
    }

    esp_camera_fb_return(fb);

    digitalWrite(flashPin, LOW);
    delay(2000);  // Let user read the result
    displayMessage("Ready");
  }

  lastButtonState = currentButtonState;
  delay(100);
}
