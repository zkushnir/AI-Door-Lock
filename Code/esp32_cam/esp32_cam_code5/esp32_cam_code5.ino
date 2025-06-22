#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>  // For parsing JSON

// WiFi credentials
const char* ssid = "Thebasement";
const char* password = "Fmab1357";

// Server endpoint
const char* serverUrl = "http://10.0.0.195:5000/upload";

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

void displayMessage(const char* message) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println(message);
  display.display();
}

// Draw your pixel art for idle state
void drawIdle() {
  display.clearDisplay();
  //id: 0 pixel 10 
  display.drawPixel(7, 15, SSD1306_WHITE);
  display.drawPixel(6, 16, SSD1306_WHITE);
  display.drawPixel(5, 19, SSD1306_WHITE);
  display.drawPixel(4, 21, SSD1306_WHITE);
  display.drawPixel(4, 23, SSD1306_WHITE);
  display.drawPixel(3, 25, SSD1306_WHITE);
  display.drawPixel(3, 26, SSD1306_WHITE);
  display.drawPixel(4, 27, SSD1306_WHITE);
  display.drawPixel(4, 28, SSD1306_WHITE);
  display.drawPixel(5, 28, SSD1306_WHITE);
  display.drawPixel(5, 27, SSD1306_WHITE);
  display.drawPixel(7, 27, SSD1306_WHITE);
  display.drawPixel(8, 26, SSD1306_WHITE);
  display.drawPixel(9, 25, SSD1306_WHITE);
  display.drawPixel(9, 24, SSD1306_WHITE);
  display.drawPixel(10, 23, SSD1306_WHITE);
  display.drawPixel(10, 21, SSD1306_WHITE);
  display.drawPixel(10, 20, SSD1306_WHITE);
  display.drawPixel(10, 22, SSD1306_WHITE);
  display.drawPixel(8, 27, SSD1306_WHITE);
  display.drawPixel(8, 29, SSD1306_WHITE);
  display.drawPixel(8, 30, SSD1306_WHITE);
  display.drawPixel(8, 31, SSD1306_WHITE);
  display.drawPixel(9, 31, SSD1306_WHITE);
  display.drawPixel(11, 31, SSD1306_WHITE);
  display.drawPixel(13, 31, SSD1306_WHITE);
  display.drawPixel(15, 31, SSD1306_WHITE);
  display.drawPixel(17, 29, SSD1306_WHITE);
  display.drawPixel(19, 28, SSD1306_WHITE);
  display.drawPixel(19, 27, SSD1306_WHITE);
  display.drawPixel(20, 26, SSD1306_WHITE);
  display.drawPixel(20, 23, SSD1306_WHITE);
  display.drawPixel(20, 21, SSD1306_WHITE);
  display.drawPixel(20, 19, SSD1306_WHITE);
  display.drawPixel(19, 18, SSD1306_WHITE);
  display.drawPixel(19, 16, SSD1306_WHITE);
  //id: 1 pixel 11 
  display.drawPixel(26, 24, SSD1306_WHITE);
  display.drawPixel(25, 25, SSD1306_WHITE);
  display.drawPixel(25, 26, SSD1306_WHITE);
  display.drawPixel(26, 26, SSD1306_WHITE);
  display.drawPixel(28, 25, SSD1306_WHITE);
  display.drawPixel(30, 25, SSD1306_WHITE);
  display.drawPixel(31, 24, SSD1306_WHITE);
  display.drawPixel(32, 24, SSD1306_WHITE);
  display.drawPixel(32, 23, SSD1306_WHITE);
  display.drawPixel(32, 22, SSD1306_WHITE);
  display.drawPixel(31, 21, SSD1306_WHITE);
  display.drawPixel(30, 20, SSD1306_WHITE);
  display.drawPixel(29, 20, SSD1306_WHITE);
  display.drawPixel(29, 21, SSD1306_WHITE);
  display.drawPixel(28, 23, SSD1306_WHITE);
  display.drawPixel(27, 24, SSD1306_WHITE);
  display.drawPixel(26, 27, SSD1306_WHITE);
  display.drawPixel(26, 28, SSD1306_WHITE);
  display.drawPixel(26, 29, SSD1306_WHITE);
  display.drawPixel(27, 29, SSD1306_WHITE);
  display.drawPixel(28, 31, SSD1306_WHITE);
  display.drawPixel(30, 32, SSD1306_WHITE);
  display.drawPixel(32, 32, SSD1306_WHITE);
  display.drawPixel(33, 32, SSD1306_WHITE);
  display.drawPixel(35, 32, SSD1306_WHITE);
  display.drawPixel(36, 31, SSD1306_WHITE);
  //id: 2 pixel 12 
  display.drawPixel(43, 14, SSD1306_WHITE);
  display.drawPixel(43, 15, SSD1306_WHITE);
  display.drawPixel(43, 17, SSD1306_WHITE);
  display.drawPixel(43, 19, SSD1306_WHITE);
  display.drawPixel(44, 19, SSD1306_WHITE);
  display.drawPixel(44, 22, SSD1306_WHITE);
  display.drawPixel(44, 24, SSD1306_WHITE);
  display.drawPixel(44, 27, SSD1306_WHITE);
  display.drawPixel(44, 30, SSD1306_WHITE);
  display.drawPixel(44, 32, SSD1306_WHITE);
  display.drawPixel(44, 33, SSD1306_WHITE);
  //id: 3 pixel 13 
  display.drawPixel(55, 24, SSD1306_WHITE);
  display.drawPixel(54, 24, SSD1306_WHITE);
  display.drawPixel(53, 24, SSD1306_WHITE);
  display.drawPixel(53, 25, SSD1306_WHITE);
  display.drawPixel(53, 26, SSD1306_WHITE);
  display.drawPixel(53, 27, SSD1306_WHITE);
  display.drawPixel(53, 28, SSD1306_WHITE);
  display.drawPixel(53, 29, SSD1306_WHITE);
  display.drawPixel(53, 30, SSD1306_WHITE);
  display.drawPixel(54, 31, SSD1306_WHITE);
  display.drawPixel(55, 31, SSD1306_WHITE);
  display.drawPixel(56, 31, SSD1306_WHITE);
  display.drawPixel(57, 31, SSD1306_WHITE);
  display.drawPixel(58, 31, SSD1306_WHITE);
  display.drawPixel(59, 30, SSD1306_WHITE);
  display.drawPixel(60, 30, SSD1306_WHITE);
  display.drawPixel(61, 29, SSD1306_WHITE);
  //id: 4 pixel 14 
  display.drawPixel(72, 24, SSD1306_WHITE);
  display.drawPixel(71, 24, SSD1306_WHITE);
  display.drawPixel(70, 24, SSD1306_WHITE);
  display.drawPixel(70, 25, SSD1306_WHITE);
  display.drawPixel(69, 26, SSD1306_WHITE);
  display.drawPixel(68, 28, SSD1306_WHITE);
  display.drawPixel(68, 29, SSD1306_WHITE);
  display.drawPixel(69, 29, SSD1306_WHITE);
  display.drawPixel(69, 30, SSD1306_WHITE);
  display.drawPixel(70, 30, SSD1306_WHITE);
  display.drawPixel(71, 30, SSD1306_WHITE);
  display.drawPixel(72, 29, SSD1306_WHITE);
  display.drawPixel(73, 29, SSD1306_WHITE);
  display.drawPixel(74, 29, SSD1306_WHITE);
  display.drawPixel(75, 28, SSD1306_WHITE);
  display.drawPixel(76, 27, SSD1306_WHITE);
  display.drawPixel(77, 27, SSD1306_WHITE);
  display.drawPixel(77, 26, SSD1306_WHITE);
  display.drawPixel(77, 25, SSD1306_WHITE);
  display.drawPixel(76, 24, SSD1306_WHITE);
  display.drawPixel(75, 24, SSD1306_WHITE);
  display.drawPixel(74, 24, SSD1306_WHITE);
  //id: 5 pixel 15 
  display.drawPixel(83, 27, SSD1306_WHITE);
  display.drawPixel(83, 29, SSD1306_WHITE);
  display.drawPixel(83, 31, SSD1306_WHITE);
  display.drawPixel(83, 32, SSD1306_WHITE);
  display.drawPixel(83, 28, SSD1306_WHITE);
  display.drawPixel(83, 23, SSD1306_WHITE);
  display.drawPixel(83, 19, SSD1306_WHITE);
  display.drawPixel(84, 18, SSD1306_WHITE);
  display.drawPixel(84, 17, SSD1306_WHITE);
  display.drawPixel(85, 18, SSD1306_WHITE);
  display.drawPixel(85, 20, SSD1306_WHITE);
  display.drawPixel(86, 21, SSD1306_WHITE);
  display.drawPixel(87, 23, SSD1306_WHITE);
  display.drawPixel(87, 24, SSD1306_WHITE);
  display.drawPixel(87, 25, SSD1306_WHITE);
  display.drawPixel(88, 26, SSD1306_WHITE);
  display.drawPixel(88, 27, SSD1306_WHITE);
  display.drawPixel(88, 28, SSD1306_WHITE);
  display.drawPixel(89, 26, SSD1306_WHITE);
  display.drawPixel(90, 24, SSD1306_WHITE);
  display.drawPixel(91, 23, SSD1306_WHITE);
  display.drawPixel(92, 22, SSD1306_WHITE);
  display.drawPixel(93, 22, SSD1306_WHITE);
  display.drawPixel(93, 23, SSD1306_WHITE);
  display.drawPixel(94, 25, SSD1306_WHITE);
  display.drawPixel(94, 27, SSD1306_WHITE);
  display.drawPixel(95, 29, SSD1306_WHITE);
  display.drawPixel(95, 31, SSD1306_WHITE);
  display.drawPixel(96, 33, SSD1306_WHITE);
  display.drawPixel(96, 34, SSD1306_WHITE);
  display.drawPixel(96, 35, SSD1306_WHITE);
  display.drawPixel(97, 34, SSD1306_WHITE);
  display.drawPixel(97, 33, SSD1306_WHITE);
  display.drawPixel(97, 32, SSD1306_WHITE);
  //id: 6 pixel 16 
  display.drawPixel(102, 27, SSD1306_WHITE);
  display.drawPixel(102, 28, SSD1306_WHITE);
  display.drawPixel(103, 28, SSD1306_WHITE);
  display.drawPixel(104, 28, SSD1306_WHITE);
  display.drawPixel(106, 27, SSD1306_WHITE);
  display.drawPixel(107, 26, SSD1306_WHITE);
  display.drawPixel(108, 25, SSD1306_WHITE);
  display.drawPixel(109, 24, SSD1306_WHITE);
  display.drawPixel(109, 23, SSD1306_WHITE);
  display.drawPixel(108, 22, SSD1306_WHITE);
  display.drawPixel(107, 22, SSD1306_WHITE);
  display.drawPixel(106, 23, SSD1306_WHITE);
  display.drawPixel(104, 24, SSD1306_WHITE);
  display.drawPixel(103, 26, SSD1306_WHITE);
  display.drawPixel(101, 29, SSD1306_WHITE);
  display.drawPixel(101, 30, SSD1306_WHITE);
  display.drawPixel(101, 31, SSD1306_WHITE);
  display.drawPixel(101, 32, SSD1306_WHITE);
  display.drawPixel(102, 32, SSD1306_WHITE);
  display.drawPixel(103, 33, SSD1306_WHITE);
  display.drawPixel(104, 34, SSD1306_WHITE);
  display.drawPixel(105, 35, SSD1306_WHITE);
  display.drawPixel(106, 35, SSD1306_WHITE);
  display.drawPixel(107, 35, SSD1306_WHITE);
  display.drawPixel(108, 35, SSD1306_WHITE);
  display.drawPixel(110, 35, SSD1306_WHITE);
  display.drawPixel(110, 34, SSD1306_WHITE);
  display.drawPixel(111, 34, SSD1306_WHITE);
  display.drawPixel(111, 33, SSD1306_WHITE);

  // Add other pixels as needed here...

  display.display();
}

void drawSmiley() {
  display.clearDisplay();
  display.drawCircle(64, 32, 30, SSD1306_WHITE);
  display.fillCircle(44, 24, 5, SSD1306_WHITE);
  display.fillCircle(84, 24, 5, SSD1306_WHITE);

  // Smile - draw 3 small lines to form a smile curve
  display.drawLine(48, 45, 56, 52, SSD1306_WHITE);
  display.drawLine(56, 52, 72, 52, SSD1306_WHITE);
  display.drawLine(72, 52, 80, 45, SSD1306_WHITE);

  display.display();
}

void drawSad() {
  display.clearDisplay();
  display.drawCircle(64, 32, 30, SSD1306_WHITE);
  display.fillCircle(44, 24, 5, SSD1306_WHITE);
  display.fillCircle(84, 24, 5, SSD1306_WHITE);

  // Sad mouth - draw 3 small lines upside down
  display.drawLine(48, 52, 56, 45, SSD1306_WHITE);
  display.drawLine(56, 45, 72, 45, SSD1306_WHITE);
  display.drawLine(72, 45, 80, 52, SSD1306_WHITE);

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

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    displayMessage("Camera fail");
    return;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(flashPin, OUTPUT);  // still set pin mode but not used

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);  // OLED failed
  }

  displayMessage("Starting WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  displayMessage("WiFi Ready");

  startCamera();

  // Show idle pixel art instead of "Ready"
  drawIdle();
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin) == LOW;

  if (currentButtonState && !lastButtonState) {
    displayMessage("Capturing...");

    // Flash LED removed per request

    // Discard first few frames for auto exposure
    for (int i = 0; i < 3; i++) {
      camera_fb_t* temp = esp_camera_fb_get();
      if (temp) esp_camera_fb_return(temp);
      delay(100);
    }

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      displayMessage("Capture fail");
      return;
    }

    displayMessage("Uploading...");
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "image/jpeg");

      int httpResponseCode = http.POST(fb->buf, fb->len);
      String payload;

      if (httpResponseCode > 0) {
        payload = http.getString();

        // Parse JSON
        StaticJsonDocument<200> doc;
        DeserializationError err = deserializeJson(doc, payload);

        if (err) {
          displayMessage("JSON Error");
        } else {
          const char* result = doc["result"];

          if (strncmp(result, "Welcome ", 8) == 0) {
              //drawSmiley();
              display.clearDisplay();
              display.setCursor(0,0);
              display.setTextSize(2);
              display.print(result);  // prints full string e.g. "Welcome Zach"
              display.display();
          } else if (strcmp(result, "Not authorised") == 0) {
              //drawSad();
              display.clearDisplay();
              display.setCursor(0, 0);
              display.setTextSize(2);
              display.println("Not authorised");
              display.display();
          } else {
              displayMessage(result);  // fallback for other messages
          }
        }
      } else {
        displayMessage("Upload fail");
      }

      http.end();
    } else {
      displayMessage("WiFi Error");
    }

    esp_camera_fb_return(fb);

    // Show idle display again after 2 sec
    delay(2000);
    drawIdle();
  }

  lastButtonState = currentButtonState;
  delay(100);
}
