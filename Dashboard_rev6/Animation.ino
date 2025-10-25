// Function to convert HSV color to RGB values, and return 16 bit color
uint16_t convert_color(int hue, int saturation, int value) {
  // Normalize ranges to 0-1
  float h = hue / 256.0;
  float s = saturation / 100.0;
  float v = value / 100.0;
  
  // Conversion formula
  // Floating point math is less efficient than integer math, but the color wheel is only calculated once at startup
  int i = (int)(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);
  
  float r, g, b;
  
  switch(i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
  
  return matrix.Color((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255)); // Return 16 bit color
}

// Initialize grid of random binary values
void setupGrid(uint8_t nextFrame[32][32]) {
  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 32; y++) {
      nextFrame[x][y] = random(2); // For each pixel, set value to 0 or 1 randomly
    }
  }
}

// Shift data from calculated frame to current frame and display
void shiftData(uint8_t grid[32][32], uint8_t nextFrame[32][32], int color) {
  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 32; y++) {
      grid[x][y] = nextFrame[x][y]; // For each pixel, shift state from next frame to current frame
      if (grid[x][y]) {
        matrix.drawPixel(x, y, color); // Draw current frame
      }
    }
  }
  matrix.show(); // Display buffer
}

void animation() { // Main function for animation app
  encoder.reset(0); // Reset encoder since its used in this app
  uint16_t wheel[256]; // Color wheel of 256 rainbow colors
  int color; // Used to select a color from the coor wheel
  uint8_t circlesOffset[32][32], sineOffset[32][32]; // Stored values for first two animations
  const float scale = 2.3; // Scale of sine wave animation

  const int kernel[][2] = {{-1, -1}, {0, -1}, {1, -1}, 
                        {-1, 0}, {1, 0}, 
                        {-1, 1}, {0, 1}, {1, 1}}; //For Conway's Game of Life

  const int kernel_dist = 1; //For natural blob generation, kernal edge length will be 2 * kernel_dist + 1. A larger number here will increase the scale of the visuals

  uint8_t grid[32][32], nextFrame[32][32]; // Storage buffers for last two animations

  for (int i = 0; i < 256; i++) {
    wheel[i] = convert_color(i, 95, 100); // Precalculate color wheel using hue 0-256, saturation 95, and value 100
  }

  for (int x = 0; x < 32; x++) { // Precalculate equation-based animations
    for (int y = 0; y < 32; y++) {
      circlesOffset[x][y] = (x*x + y*y) % 256; // Corner circles
      sineOffset[x][y] = (int)(64 * (sin(x / scale) + sin(y / scale)) + 128); // Dual axis sine wave
    }
  }

  while (true) {
    int position = encoder.readEncoder();
    position = mod(position, 4);
    switch (position) {
      case 0: // Corner circles animation using quadratic function
          for (int i = 0; i < 256; i += 8) { // Loop through each frame of the animation
            for (int x = 0; x < 32; x++) {
              for (int y = 0; y < 32; y++) {
                color = (circlesOffset[x][y] + i) % 256; // For each pixel, reference the precalculated values
                matrix.drawPixel(x, y, wheel[color]);
              }
            }

            matrix.show();
            if (encoder.isEncoderButtonClicked()) {
              encoder.reset(0); // Exit to dashboard if the encoder is pressed
              return;
            }
          }
        break;

      case 1: // Waveform animation using trig function
          for (int i = 0; i < 256; i += 8) { // Loop through each frame of the animation
            for (int x = 0; x < 32; x++) {
              for (int y = 0; y < 32; y++) {
                color = (sineOffset[x][y] + i) % 256; // For each pixel, reference the precalculated values
                matrix.drawPixel(x, y, wheel[color]);
              }
            }
            matrix.show();
            if (encoder.isEncoderButtonClicked()) {
              encoder.reset(0); // Exit to dashboard if the encoder is pressed
              return;
            }
          }
        break;

      case 2: // Conway's game of life
        setupGrid(nextFrame); // Start frame with random binary values

        // Choose a random color
        color = wheel[random(256)];

        for (int iteration = 0; iteration < 50; iteration++) { // Run 50 iterations for each round
          matrix.clear();

          //shift data to primary array and display
          shiftData(grid, nextFrame, color); // Display new frame

          if (encoder.isEncoderButtonClicked()) {
            encoder.reset(0); // Exit to dashboard if the encoder is pressed
            return;
          }
          delay(30);

          //calculate next iteration
          for (int x = 0; x < 32; x++) {
            for (int y = 0; y < 32; y++) {
              //for each cell, count number of alive cells within kernal range
              uint8_t count = 0;

              for (int i = 0; i < 8; i++) { // Loop over the cells within the kernal
                  if (x + kernel[i][0] >= 0 && x + kernel[i][0] <= 31 && y + kernel[i][1] >= 0 && y + kernel[i][1] <= 31) { // Check whether the cell in the kernal is within the grid
                    count += grid[x + kernel[i][0]][y + kernel[i][1]]; // If the cell is alive (1), increase the count
                  }
              }

              //determine new cell state
              if (count < 2 || count > 3) {
                nextFrame[x][y] = 0; // The cell dies due to underpopulation/overpopulation
              }
              else if (count == 3) {
                nextFrame[x][y] = 1; // A new cell is born if the population is exactly 3
              }
              // Otherwise, the cell keeps its current state
            }
          }
        }
        break;

      case 3: // Organic shape generation using majority-rule smoothing
        setupGrid(nextFrame); // Start frame with random binary values
        
        // Choose a random color
        color = wheel[random(256)];

        for (int iteration = 0; iteration < 20; iteration++) {
          matrix.clear();
          //shift data to primary array and display
          shiftData(grid, nextFrame, color); // Display new frame

          if (encoder.isEncoderButtonClicked()) {
              encoder.reset(0); // Exit to dashboard if the encoder is pressed
              return;
          }
          delay(20);

          //calculate next iteration
          for (int x = 0; x < 32; x++) {
            for (int y = 0; y < 32; y++) {
              //for each pixel, count total number of pixels within kernal range, and number of active pixels
              uint8_t count = 0, total = 0;

              for (int i = -1 * kernel_dist; i <= kernel_dist; i ++) {
                for (int j = -1 * kernel_dist; j <= kernel_dist; j ++) { // Kernal can be changed to adjust scale of the animation
                  if (x + i >= 0 && x + i <= 31 && y + j >= 0 && y + j <= 31) { // Check if the pixel is within the grid
                    total++; // Increase the total
                    count += grid[x + i][y + j]; // Increase the count if the pixel is on (1)
                  }
                }
              }

              //determine new pixel color
             nextFrame[x][y] = 2 * count / total >= 1; // The pixel will be on if at least half of the surrounding pixels are on
            }
          }
        }
        break;
    }
  }
}