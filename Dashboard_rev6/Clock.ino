float offset = 0; // Stores manual time offset for the clock

void wifiClock() { // Main function for clock app
  encoder.setAcceleration(40); // Increased acceleration to allow for both fine tuning and fast rotation
  while (true) {
    // Get current time
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    float seconds = timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;

    matrix.clear();
    matrix.drawRGBBitmap(0, 0, (const uint16_t *) clockface, 32, 32); // Draw analog clock face

    drawHand(TWO_PI * (seconds + offset) / 43200 - HALF_PI, 7.5, RED); //hour hand
    drawHand(TWO_PI * (seconds + offset) / 3600 - HALF_PI, 9.5, WHITE); //minute hand
    matrix.show();

    uint32_t timestamp = millis();
    while (millis() < timestamp + 9000) { // Wait nine seconds while checking inputs
      if (encoder.isEncoderButtonClicked()) {
        offset = clockAdjust(seconds); // If the encoder is pressed, enter manual adjusting mode
        break;
      }
      if (!digitalRead(BUTTON)) { // If the joystick is pressed, exit and return to dashboard
        encoder.setAcceleration(10); // Reset acceleration
        encoder.reset(1); // Set the encoder position to 1 since it may have been moved while the clock app is open
        return;
      }
    }
  }
}

// Function for manually adjusting the time
float clockAdjust(float startSeconds) {
  float offsetSeconds;
  encoder.reset(0);
  while (!encoder.isEncoderButtonClicked()) { // When the encoder is pressed, return to normal mode
    offsetSeconds = encoder.readEncoder() * 60; // Each "click" of the encoder moves the time by 60 seconds

    matrix.clear();
    matrix.drawRGBBitmap(0, 0, (const uint16_t *) clockface, 32, 32); // Draw the clock face background

    // Draw triangles in the corners to show that the clock can be adjusted
    matrix.fillTriangle(0, 0, 0, 3, 3, 0, CYAN);
    matrix.fillTriangle(31, 0, 28, 0, 31, 3, CYAN);
    matrix.fillTriangle(0, 31, 0, 28, 3, 31, CYAN);
    matrix.fillTriangle(31, 31, 28, 31, 31, 28, CYAN);

    drawHand(TWO_PI * (startSeconds + offsetSeconds) / 43200 - HALF_PI, 7.5, RED); //hour hand with shorter length
    drawHand(TWO_PI * (startSeconds + offsetSeconds) / 3600 - HALF_PI, 9.5, WHITE); //second hand with longer length
    matrix.show();
  }
  return offsetSeconds;
}

void drawHand(float angle, float radius, uint16_t color) {
  int x = round(15.5 + radius * cos(angle)), y = round(15.5 + radius * sin(angle)); // Use trigonometry to calculate clock hand position

  // Draw four lines that make up each clock hand
  matrix.drawLine(15, 15, x, y, color);
  matrix.drawLine(16, 15, x, y, color);
  matrix.drawLine(15, 16, x, y, color);
  matrix.drawLine(16, 16, x, y, color);
}