// Variables to store game values
int x, y, button, snakeLength;

// NOTE: Defined globally instead of inside snakeGame()
// Large arrays on the ESP32 can cause stack overflow if allocated locally
// Keeping snake[] in global scope puts it in DRAM instead of the limited task stack
int snake[900][2]; // This stores each segment of the snake

void snakeMenu() { // Main snake game function
  while (true) {
    snakeGame(); // Start the game

    // Display score screen when game is over
    matrix.clear();
    matrix.setCursor(1, 3);
    matrix.setTextSize(1);
    matrix.setTextColor(MAGENTA);
    matrix.println("Score");

    // Shift text location based on width
    if (snakeLength < 10) {
      matrix.setCursor(11, 14);
    }
    else {
      matrix.setCursor(5, 14);
    }

    // Print score to matrix
    matrix.setTextSize(2);
    matrix.setTextColor(YELLOW);
    matrix.println(snakeLength);
    matrix.show();

    while (digitalRead(BUTTON)) {
      if (encoder.isEncoderButtonClicked()) {
        return; // Exit to dashboard if the encoder is pressed
      }
    }
  }
}

void snakeGame() {
  // Define snake starting position
  int direction[] = {1, 0}, food[] = {20, 20}, head[2] = {5, 5};
  snakeLength = 4;
  
  // Define where each segment starts
  snake[0][0] = head[0];
  snake[0][1] = head[1];

  snake[1][0] = head[0] - 1;
  snake[1][1] = head[1];

  snake[2][0] = head[0] - 2;
  snake[2][1] = head[1];

  snake[3][0] = head[0] - 3;
  snake[3][1] = head[1];

  while (true) { // Main game loop
    matrix.clear();

    // Use reading from joystick to determine new direction
    if (direction[0] == 0) {
      x = analogRead(JOYSTICK_X);
      if (x < 1750) { // On the ESP32, analogRead returns values of 0-4096, so 1750 and 1950 were chosen as values that are off the midpoint of the joystick
        direction[0] = -1; // Current direction is stored as x, y
        direction[1] = 0;
      }
      else if (x > 1950) {
        direction[0] = 1;
        direction[1] = 0;
      }
    }
    // We only need to check one axis since the snake cannot change the direction that it is currently traveling
    // for example, if it is moving left the direction can only change to up or down, since it cannot go backwards
    else {
      y = analogRead(JOYSTICK_Y);
      if (y < 1750) {
        direction[0] = 0;
        direction[1] = -1;
      }
      else if (y > 1950) {
        direction[0] = 0;
        direction[1] = 1;
      }
    }

    // Move the head of the snake one unit in the current direction
    head[0] += direction[0];
    head[1] += direction[1];

    // Check for collision with walls
    if (head[0] <= 0 || head[0] >= 31 || head[1] <= 0 || head[1] >= 31) {
      return; // Exit to score screen
    }

    // Shift all segments down the array
    for (int i = snakeLength; i > 0; i--) {
      snake[i][0] = snake[i - 1][0];
      snake[i][1] = snake[i - 1][1];
    }

    // Check for collision with self
    for (int i = 1; i < snakeLength; i ++) {
      if (head[0] == snake[i][0] && head[1] == snake[i][1]) {
        return; // Exit to score screen
      }
    }

    // Set the first segment of the snake to the head position
    snake[0][0] = head[0];
    snake[0][1] = head[1];

    // Check for collision with food
    if (head[0] == food[0] && head[1] == food[1]) {
      snakeLength ++; // If the head overlaps with the food, increase the snake length by one
      bool valid = false;
      while (!valid) { // Keep trying to generate food until its in a valid position
        valid = true;
        food[0] = random(1, 31);
        food[1] = random(1, 31);
        for (int i = 0; i < snakeLength; i ++) { // Check each segment of the snake
          if (food[0] == snake[i][0] && food[1] == snake[i][1]) {
            valid = false; // If the food overlaps with any segment of the snake, try again
          }
        }
      }
    }

    // Display all segments of the snake
    for (int i = 1; i < snakeLength; i ++) {
      matrix.drawPixel(snake[i][0], snake[i][1], GREEN);
    }

    // Display the head in yellow
    matrix.drawPixel(snake[0][0], snake[0][1], YELLOW);

    // Draw the food
    matrix.drawPixel(food[0], food[1], RED);

    // Draw the border
    matrix.drawRect(0, 0, 32, 32, CYAN);
    matrix.show();
    delay(70); // Delay for correct game speed
  }
}