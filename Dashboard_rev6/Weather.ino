// OpenWeatherMap API info
String apiKey = "800b41b115fd511600955dadd24e2710";  
String city = "Raleigh";
String countryCode = "US";
String units = "metric";

void weather() {
  while (true) {
    if (WiFi.status() == WL_CONNECTED) { // Check to see if wifi is connected
      HTTPClient http;
      String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=" + units; // Define url

      http.begin(url);
      int httpCode = http.GET();

      if (httpCode > 0) { // Check if data is received
        String payload = http.getString();
        Serial.println("Received payload:");
        Serial.println(payload); // Print received data

        // Allocate buffer for ArduinoJson
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) { // Check is data is processed correctly
          // Extract temperature and weather code
          float temperature = doc["main"]["temp"];
          int code = doc["weather"][0]["id"];

          matrix.clear();
          // All 8 weather icons are stored on a 256 x 13 bitmap, which is moved to display a specific portion
          int offset;
          switch (code / 100) { // Determine offset for the icon bitmap depending on the weather code
            case 2: // Thunderstorm (2xx)
              offset = 0;
              break;
            case 3: // Drizzle (3xx)
              offset = -32;
              break;
            case 5: // Rain (5xx)
              offset = -64;
              break;
            case 6: // Snow (6xx)
              offset = -96;
              break;
            case 7: // Atmosphere (7xx)
              offset = -128;
              break;
            case 8:
              if (code == 800) { // Clear (800)
                offset = -160;
              }
              else if (code == 801 || code == 802) { // Clouds (80x)
                offset = -192;
              }
              else {
                offset = -224;
              }
              break;
          }
          // Display correctly positioned icon bitmap
          matrix.drawRGBBitmap(offset, 18, (const uint16_t *) weathericons, 256, 13);
          // White border
          matrix.drawRect(0, 0, 32, 32, WHITE);

          struct tm timeinfo;
          getLocalTime(&timeinfo); // Get current time

          if (timeinfo.tm_hour < 10) // Shift display position based on current hour
          {
            matrix.setCursor(4, 2);
          }
          else {
            matrix.setCursor(1, 2);
          }
          
          // Display time
          matrix.setTextSize(1);
          matrix.setTextColor(YELLOW);
          matrix.print(timeinfo.tm_hour);
          matrix.print(":");
          if (timeinfo.tm_min < 10) {
            matrix.print("0"); // Add a zero for correct formatting if the minute is a single digit
          }
          matrix.print(timeinfo.tm_min);

          // Display temperature
          matrix.setCursor(4, 10);
          matrix.setTextColor(CYAN);
          matrix.print((int) round(temperature));
          matrix.print((char)247); // Workaround for degree symbol
          matrix.print("C");
          matrix.show();
        } 
        else {
          Serial.print("JSON parse error: "); // JSON error handling
          Serial.println(error.c_str());
        }
      } 
      else {
        Serial.print("HTTP request failed, code: "); // HTTP error handling
        Serial.println(httpCode);
      }

      http.end();
    }

    uint32_t timestamp = millis();
    while (millis() < timestamp + 9000) { // Wait nine seconds while checking input
      if (encoder.isEncoderButtonClicked()) {
        return; // Exit to dashboard if the encoder is pressed
      }
    }
  }
}