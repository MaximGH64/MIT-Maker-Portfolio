// Calibrated colors of each side of the rubik's cube
const rgbcolor red = {187, 37, 35};
const rgbcolor orange = {165, 52, 33};
const rgbcolor yellow = {115, 89, 36};

const rgbcolor green = {67, 120, 47};
const rgbcolor blue = {50, 90, 102};
const rgbcolor white = {93, 87, 60};

// Store order of colors in an array for efficiency
const rgbcolor colors[] = {red, orange, yellow, green, blue, white};
const uint16_t display[] = {RED, ORANGE, YELLOW, GREEN, BLUE, WHITE};


rgbcolor getColor() {
  uint16_t r, g, b, c;

  // The sensor outputs three color channels (r, g, b) and a brightness channel c
  tcs.getRawData(&r, &g, &b, &c);

  // Prevent division by zero
  if (c == 0) c = 1;

  // Divide each color chanel by total brightness and normalize to 0–255
  rgbcolor result = {(uint8_t) min(255, (r * 255) / c), (uint8_t) min(255, (g * 255) / c), (uint8_t) min(255, (b * 255) / c)};
  return result; // An rgbcolor struct is used to return all color channels from one function
}

void colorSense() { // Main function for color sensor app
  matrix.setTextSize(1);
  while (true) {
    matrix.clear();
    rgbcolor color = getColor(); // Get the color reading
    for (int i = 0; i < 6; i++) { // Check if any of the 6 colors is being detected
      // Calculate distance between the reading and each defined color
      int dr = color.r - colors[i].r; 
      int dg = color.g - colors[i].g;
      int db = color.b - colors[i].b;

      int dist = dr*dr + dg*dg + db*db; // Distance formula in 3d, no square root for optimization

      if (dist <= 100) {  // Checking if the distance is less than 10 (100 is used here since its 10^2)
        matrix.drawRect(0, 0, 32, 32, display[i]); // If the current color is detected, draw a rectangle of the same color
        break; // Break out of the loop since only one color can be detected at a time
      }
    }

    // Display current RGB values
    matrix.setTextColor(RED);
    matrix.setCursor(4, 3);
    matrix.print("R");
    matrix.print(color.r);

    matrix.setTextColor(GREEN);
    matrix.setCursor(4, 12);
    matrix.print("G");
    matrix.print(color.g);

    matrix.setTextColor(BLUE);
    matrix.setCursor(4, 21);
    matrix.print("B");
    matrix.print(color.b);
    matrix.show();
    delay(30);
    if (encoder.isEncoderButtonClicked()) {
        return; // Exit to dashboard if the encoder is pressed
    }
  }
}