// Callback function when data is received
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  // Check if received the expected amount of data
  if (len == sizeof(payload)) {
    memcpy(&payload, incomingData, sizeof(payload)); // Store received data in payload struct
  }
}

void wireless() {
  if (esp_now_init() != ESP_OK) { // Start ESP NOW
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the callback
  esp_now_register_recv_cb(onReceive);

  while (true) { // Main update loop
    matrix.clear();
    matrix.setTextSize(1);
    
    if (payload.distance == -1) { // The distance wsensor returns -1 if a reading fails
      matrix.setCursor(1, 1);
      matrix.setTextColor(ORANGE);
      matrix.print("Error"); // Display error to matrix
    }

    else { // Conditionals to keep text centered
      if (payload.distance < 10) {
        matrix.setCursor(7, 1);
      }
      else if (payload.distance < 100) {
        matrix.setCursor(4, 1);
      }
      else {
        matrix.setCursor(1, 1);
      }
      matrix.setTextColor(GREEN);
      matrix.print(payload.distance);
      matrix.print("cm");
    }

    // Border around climate information
    matrix.drawRoundRect(0, 10, 32, 22, 7, CYAN);

    matrix.setCursor(5, 13);
    matrix.setTextColor(YELLOW);
    matrix.print(payload.temperature); // Display temperature
    matrix.print((char)247); // Workaround for degree symbol
    matrix.print("C");

    matrix.setCursor(8, 22);
    matrix.print(payload.humidity); // Display humidity
    matrix.print("%");

    matrix.show();
    delay(50);

    if (encoder.isEncoderButtonClicked()) {
      return; // Exit to dashboard if the encoder is pressed
    }
  }
}