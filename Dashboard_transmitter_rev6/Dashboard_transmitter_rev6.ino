// Import Libraries
#include <esp_now.h> // Wireless connection to other controller
#include <WiFi.h>
#include <HCSR04.h>
#include <DHT.h>

// Define ultrasonic distance sensor pins
#define TRIG 33
#define ECHO 32

#define DHTPIN 27       // Define the pin used to connect the temperature and humidity sensor
#define DHTTYPE DHT11  // Define the sensor type

// Define built in LED pin
#define LED 2

// Distance sensor setup
UltraSonicDistanceSensor distanceSensor(TRIG, ECHO);

// Temp/humidity sensor setup
DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials
const char* ssid     = "MAX";
const char* password = "hollandlop";

uint8_t receiverAddress[] = {0x38, 0x18, 0x2B, 0xE9, 0x86, 0xD4}; // MAC of other ESP32

// Typedef for wireless data transmission

typedef struct __attribute__((packed)) data_message {
  int16_t distance;
  uint8_t temperature;
  uint8_t humidity;
} data_message;

data_message packet;

void setup() {
  Serial.begin(115200);
  // The built-in LED is used to display connection information
  pinMode(LED, OUTPUT);

  digitalWrite(LED, HIGH); // Indicate startup and wait 1 second before connecting to wifi
  delay(200);
  digitalWrite(LED, LOW);
  delay(800);

  WiFi.mode(WIFI_STA);

  // Connect to Wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, HIGH); // LED is on to show wifi connecting
    delay(500);
  }

  if (esp_now_init() != ESP_OK) { // Start ESP-NOW
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {}; // Setup wireless connection with the other controller
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  dht.begin();  // Initialize the DHT sensor
}

void loop() {
  packet.distance = distanceSensor.measureDistanceCm(); // Measure distance and store result in the packet struct
  Serial.println(packet.distance);

  packet.temperature = dht.readTemperature();
  packet.humidity = dht.readHumidity(); // Read temperature (Celsius) and store result in the packet struct
  Serial.println(packet.temperature);
  Serial.println(packet.humidity); // Read humidity and store result in the packet struct

  digitalWrite(LED, HIGH); // Flash LED to indicate data transmission
  esp_now_send(receiverAddress, (uint8_t *) &packet, sizeof(packet)); // Send data in discrete bytes
  delay(100);
  digitalWrite(LED, LOW);
  delay(200);
}
