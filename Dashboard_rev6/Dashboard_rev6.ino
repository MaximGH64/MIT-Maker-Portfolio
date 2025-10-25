/*
PORTFOLIO PROJECT: Modular LED Dashboard System
KEY FEATURES:
- 6 interactive applications
- Wireless sensor data transmission
- Real-time animations and games
- API integration for weather data
- Color recognition system
TECHNICAL HIGHLIGHTS:
- ESP-NOW wireless protocol
- Multi-threaded sensor polling
- Memory-optimized graphics
- Modular code architecture
*/

// Import Libraries
#include <Adafruit_GFX.h> // Graphics
#include <Adafruit_NeoMatrix.h> // Allows use of the graphics library with LED matrix
#include <Adafruit_NeoPixel.h> // RGB LED Control
#include <AiEsp32RotaryEncoder.h>
#include <WiFi.h>
#include <Adafruit_TCS34725.h> // Color Sensor
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_now.h> // Wireless connection to other controller
#include "time.h"

// Import Images
#include "clockface.h"
#include "wificonnect.h"
#include "homepage.h"
#include "weathericons.h"

// Define joystick pins
#define PIN 4
#define JOYSTICK_X 35
#define JOYSTICK_Y 34
#define BUTTON 27

// Brightness of matrix ranges from 0-255
#define BRIGHTNESS 64

// Define width and height of each panel
#define mw 32
#define mh 8

// Define rotary encoder pins
#define ROTARY_ENCODER_A_PIN 32 
#define ROTARY_ENCODER_B_PIN 33
#define ROTARY_ENCODER_BUTTON_PIN 25
#define ROTARY_ENCODER_VCC_PIN -1 // not needed
#define ROTARY_ENCODER_STEPS 4

// Panel Setup
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(mw, mh, 1, 4, PIN, NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS + NEO_TILE_ZIGZAG + NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);

// Rotary Encoder Setup
AiEsp32RotaryEncoder encoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

// Color Sensor Setup
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

// WiFi credentials
const char* ssid     = "MAX";
const char* password = "hollandlop";

// NTP server and timezone
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400; // Example: -14400 for EDT (UTC-4)
const int   daylightOffset_sec = 0; // 3600 if Daylight Saving Time is active, else 0

// Common colors
const uint16_t BLACK = matrix.Color(0, 0, 0);
const uint16_t RED = matrix.Color(255, 0, 0);
const uint16_t GREEN = matrix.Color(0, 255, 0);
const uint16_t BLUE = matrix.Color(0, 0, 255);
const uint16_t YELLOW = matrix.Color(255, 255, 0);
const uint16_t CYAN = matrix.Color(0, 255, 255);
const uint16_t MAGENTA = matrix.Color(255, 0, 255);
const uint16_t ORANGE = matrix.Color(255, 128, 0);
const uint16_t WHITE = matrix.Color(255, 255, 255);

// UI layout definition
const int selectPositions[][2] = {{0, 0}, {16, 0}, {0, 10}, {16, 10}, {0, 21}, {16, 21}};
const int ySize[] = {11, 11, 12, 12, 11, 11};

// Typedef for color sensor processing
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgbcolor;

// Typedef for wireless data transmission
typedef struct __attribute__((packed)) data_message {
  int16_t distance; // Signed integer for distance since -1 is used as an error code
  uint8_t temperature;
  uint8_t humidity;
} data_message;

data_message payload;

// Custom function to calculate arithmetic mod; % behaves differently for negative inputs
int mod(int num, int divisor) {
    return ((num % divisor) + divisor) % divisor;
}

void setup() {
  Serial.begin(115200);

  // Setup Joystick inputs
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  // Setup LED matrix
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
  matrix.setRotation(1); // Orientation can be changed to cardinal directions, 0-3
  matrix.clear();

  // Wifi mode for ESP-NOW communication
  WiFi.mode(WIFI_STA);

  // Connect to Wifi
  WiFi.begin(ssid, password);
  matrix.drawRGBBitmap(0, 0, (const uint16_t *) wificonnect, 32, 32); // Display wifi connecting screen
  matrix.show();
  while (WiFi.status() != WL_CONNECTED) { // Wait wuntil wifi connects
    delay(500);
  }

  // Setup rotary encoder
  encoder.begin();
  encoder.setup([]{ encoder.readEncoder_ISR(); });
  encoder.setAcceleration(10); // tweak value (10–100)

  // Setup color sensor
  Wire.begin(21, 22);  // SDA, SCL pins for ESP32

  if (!tcs.begin()) {
    Serial.println("color sensor not found");
  } 

  // Initialize and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  int position = encoder.readEncoder();
  position = mod(position, 6); // Normalize position to 0-6 to display UI for selecting apps

  matrix.clear();
  matrix.drawRGBBitmap(0, 0, (const uint16_t *) homepage, 32, 32);
  matrix.drawRect(selectPositions[position][0], selectPositions[position][1], 16, ySize[position], WHITE);
  matrix.show();

  if (encoder.isEncoderButtonClicked()) {
    switch (position) { // When the rotary encoder is pressed, start the app that is currently selected
      case 0:
        Serial.println("Starting Animation App");
        animation();
        break;
      case 1:
        Serial.println("Starting Clock App");
        wifiClock();
        break;
      case 2:
        Serial.println("Starting Game App");
        snakeMenu();
        break;
      case 3:
        Serial.println("Starting Color Sensor App");
        colorSense();
        break;
      case 4:
        Serial.println("Starting Weather App");
        weather();
        break;
      case 5:
        Serial.println("Starting Sensor App");
        wireless();
        break;
    }
  }
}