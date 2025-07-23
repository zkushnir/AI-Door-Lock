#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Thebasement";
const char* password = "Fmab1357";
// Server URL for image upload
const char* serverUrl = "http://10.0.0.195:5000/upload";

// Pin definitions
const int buttonPin = 12;     // Wake button (also used as trigger)
const int flashPin = 4;       // Flash LED (currently unused in logic)
const int oledPowerPin = 15;  // Transistor control pin for OLED power
#define OLED_SDA 14           // SDA pin for OLED I2C communication
#define OLED_SCL 2            // SCL pin for OLED I2C communication

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Initialize the OLED display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// State variables for OLED management
bool oledOn = false;
unsigned long oledOnTime = 0; // Timestamp when OLED was turned on
// bool lastButtonState = LOW; // Not used in this version

// Function to display a message on the OLED
void displayMessage(const char* message) {
  display.clearDisplay();         // Clear the entire display buffer
  display.setCursor(0, 0);        // Set cursor to top-left corner
  display.setTextSize(2);         // Set text size (2x default)
  display.setTextColor(SSD1306_WHITE); // Set text color to white
  display.println(message);       // Print the message
  display.display();              // Push the buffer to the display
}

// Function to draw the "Ready" message when idle
void drawIdle() {
  display.clearDisplay();
  display.setCursor(10, 20);
  display.setTextSize(2);
  display.println("Ready");
  display.display();
}

// Function to configure and start the camera
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
  config.pin_reset = -1; // Reset pin not used
  config.xclk_freq_hz = 20000000; // XCLK frequency
  config.pixel_format = PIXFORMAT_JPEG; // Output format

  // Configure frame size and JPEG quality based on PSRAM availability
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA; // 640x480
    config.jpeg_quality = 10;          // Lower quality = smaller file
    config.fb_count = 1;               // Number of frame buffers
  } else {
    config.frame_size = FRAMESIZE_CIF; // 352x288
    config.jpeg_quality = 12;          // Higher quality = larger file
    config.fb_count = 1;
  }

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    displayMessage("Cam Error");
    // Consider adding a loop here or a more robust error handling
    // if camera initialization is critical for operation.
  }
}

void setup() {
  Serial.begin(115200); // Initialize serial communication for debugging

  // Configure pin modes
  pinMode(buttonPin, INPUT_PULLUP);  // Button with internal pull-up resistor
  pinMode(flashPin, OUTPUT);         // Flash LED pin
  pinMode(oledPowerPin, OUTPUT);     // OLED power control pin

  // Enable external wake-up from deep sleep using the button
  // 0 means wake on LOW level (button pressed)
  esp_sleep_enable_ext0_wakeup((gpio_num_t)buttonPin, 0);

  // Check if the device woke up from deep sleep due to the button.
  // If not, and it's a fresh boot, go to deep sleep immediately.
  if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Not a wake-up from deep sleep. Entering deep sleep.");
    esp_deep_sleep_start();
  }

  // If we reach here, it means the device woke up from deep sleep via the button.
  Serial.println("Woke up from deep sleep.");

  // Turn on OLED power
  digitalWrite(oledPowerPin, HIGH);
  oledOnTime = millis(); // Record the time OLED was turned on
  oledOn = true;         // Set OLED state to on

  // Initialize I2C communication for OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  // Initialize OLED display with I2C address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1); // Don't proceed if OLED fails to initialize
  }

  displayMessage("Hold..."); // Show "WiFi..." on OLED
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password); // Start Wi-Fi connection
  // Wait until Wi-Fi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(300); // Short delay to avoid busy-waiting
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  //displayMessage("WiFi Ready"); // Show "WiFi Ready" on OLED

  startCamera(); // Initialize the camera
  drawIdle();    // Display "Ready" message
}

void loop() {
  // This section for OLED auto-off is less critical since the device will deep sleep,
  // but it's kept for completeness in case the deep sleep is removed later or for debugging.
  if (oledOn && (millis() - oledOnTime > 60000)) {
    display.clearDisplay(); // Clear the display buffer
    display.display();      // Push the empty buffer to turn off pixels
    digitalWrite(oledPowerPin, LOW); // Cut power to the OLED via transistor
    oledOn = false;         // Update OLED state
    Serial.println("OLED turned off to save power.");
  }

  // Since we are waking from deep sleep on button press,
  // the button will already be LOW when loop() starts if it was the wake source.
  // We don't need to continuously check digitalRead(buttonPin) in loop() for the primary action.
  // The first execution of loop() after wake-up will handle the button press.

  // The button is pressed (LOW because of INPUT_PULLUP)
  // This block will execute once after waking from deep sleep.
  // No need for an explicit 'if (digitalRead(buttonPin) == LOW)' here for the main action
  // because the device only wakes up *when* the button is pressed.
  // However, keeping it makes the logic clearer if you decide to add other non-sleep functionality.
  // For now, we'll assume the button press is the reason for being awake.

  // Re-enable OLED if it was off (this happens on wake-up)
  if (!oledOn) {
    digitalWrite(oledPowerPin, HIGH);
    oledOnTime = millis();
    oledOn = true;
    Serial.println("OLED turned back on.");
  }
  displayMessage("Capturing...");
  Serial.println("Woke up. Capturing image...");

  // Take a few dummy frames to allow camera to stabilize (optional but good practice)
  for (int i = 0; i < 3; i++) {
    camera_fb_t* temp = esp_camera_fb_get();
    if (temp) esp_camera_fb_return(temp);
    delay(100);
  }

  // Get the actual camera frame buffer
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    displayMessage("Capture fail");
    Serial.println("Camera capture failed!");
    delay(2000); // Display message for 2 seconds
    // Clear display before deep sleep
    display.clearDisplay();
    display.display();
    digitalWrite(oledPowerPin, LOW); // Cut power to the OLED
    esp_deep_sleep_start(); // Go back to deep sleep on failure
  }

  displayMessage("Uploading...");
  Serial.println("Image captured. Uploading...");

  // Wi-Fi connection is established in setup() after wake-up.
  // We assume it's connected here. If not, the initial connect loop in setup() would have handled it.
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl); // Specify the server URL
    http.addHeader("Content-Type", "image/jpeg"); // Set content type for JPEG image

    // Perform the HTTP POST request with the image data
    int httpResponseCode = http.POST(fb->buf, fb->len);
    String payload;

    if (httpResponseCode > 0) {
      payload = http.getString(); // Get the response payload (JSON)
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
      Serial.printf("Server Response: %s\n", payload.c_str());

      StaticJsonDocument<200> doc; // Create a JSON document object
      // Deserialize the JSON payload
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        displayMessage("JSON Error"); // Show error if JSON parsing fails
        Serial.printf("deserializeJson() failed: %s\n", error.c_str());
      } else {
        // Extract the "result" field from the JSON
        const char* result = doc["result"];
        if (result) {
          displayMessage(result); // Display the result from the server
          Serial.printf("Result from server: %s\n", result);
        } else {
          displayMessage("No Result"); // Handle case where "result" field is missing
          Serial.println("Server response did not contain 'result' field.");
        }
      }
    } else {
      displayMessage("Upload fail"); // Show upload failure message
      Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end(); // Close the HTTP connection
  } else {
    displayMessage("WiFi Error"); // Show Wi-Fi error if not connected (shouldn't happen often if setup() works)
    Serial.println("Wi-Fi not connected for upload.");
  }

  esp_camera_fb_return(fb); // Return the frame buffer to the camera
  delay(2000);              // Keep the message on display for 2 seconds

  // Clear display before deep sleep
  display.clearDisplay();
  display.display();
  digitalWrite(oledPowerPin, LOW); // Cut power to the OLED via transistor
  oledOn = false; // Update OLED state

  Serial.println("Operation complete. Entering deep sleep.");
  esp_deep_sleep_start(); // Enter deep sleep to save power
}
