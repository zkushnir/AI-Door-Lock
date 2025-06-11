#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "Thebasement";
const char* password = "Fmab1357";

// Server endpoint
const char* serverUrl = "http://10.0.0.136:5000/upload";

// GPIO for the button and flash
const int buttonPin = 12;
const int flashPin = 4;  // GPIO 4 controls the flash (LED)

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
    config.fb_count = 1;  // force single buffer to avoid stale frames
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);  // button connected to GND when pressed
  pinMode(flashPin, OUTPUT);         // flash LED pin

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  startCamera();
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin) == LOW;

  if (currentButtonState && !lastButtonState) {
    Serial.println("Button pressed. Flushing old frames...");

    // Flash ON
    digitalWrite(flashPin, HIGH);
    delay(100);  // Let LED illuminate scene

    // Flush old frames
    for (int i = 0; i < 3; i++) {
      camera_fb_t* temp = esp_camera_fb_get();
      if (temp) esp_camera_fb_return(temp);
      delay(100);
    }

    // Capture fresh image
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      digitalWrite(flashPin, LOW);  // Flash OFF
      return;
    }

    Serial.println("Photo captured. Uploading...");
    Serial.printf("Captured size: %d bytes\n", fb->len);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "image/jpeg");

      int httpResponseCode = http.POST(fb->buf, fb->len);
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
      http.end();
    }

    esp_camera_fb_return(fb);

    // Flash OFF
    digitalWrite(flashPin, LOW);
    delay(500);  // let system cool down before next capture
  }

  lastButtonState = currentButtonState;
  delay(100);
}
