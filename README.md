#Modular LED Dashboard System
Link: https://github.com/MaximGH64/MIT-Maker-Portfolio
Project Summary: A modular LED dashboard system with 6 interactive applications, wireless sensor integration, and real-time generative animations. Built on the ESP32 platform with a 32x32 addressable LED matrix.
##Project Motivation
Over 5+ years of working with Arduino and embedded systems, I developed expertise in various sensors, displays, and wireless communication protocols. This project combines that knowledge into a single, cohesive system, collecting and refining numerous concepts I've experimented with over time into a single, polished, and functional dashboard.
##Core Features
-Generative Art Engine: Runs complex cellular automata (like Conway's Game of Life) and procedural animations.
-WiFi-Connected Clock: Syncs with an NTP server for accurate time and features manual adjustment.
-Snake Game: A complete, playable Snake game controlled by an analog joystick.
-Color-Sensing App: Uses a TCS34725 sensor to detect and identify real-world colors (calibrated for a Rubik's Cube).
-Live Weather Display: Fetches and displays real-time weather data from the OpenWeatherMap API.
-Wireless Sensor Dashboard: Receives and displays data (distance, temperature, humidity) from a separate, battery-powered transmitter module via ESP-NOW.
##System Architecture
The project is built as two distinct modules that communicate wirelessly.
1. Main Display Module (Receiver)
This is the main user-facing unit. It runs the 6-app "operating system" and controls all local hardware.
Controller: ESP32
Display: 32x32 (1024-pixel) WS2812B Addressable LED Matrix (four 8x32 panels)
Inputs:
Rotary Encoder: For menu navigation and fine-tuning (e.g., clock adjust).
Joystick: For analog control, primarily used in the Snake game.
Sensor: TCS34725 Color Sensor with built-in white LED.
Wireless: ESP-NOW (to receive data from transmitter) & Wi-Fi (for API/NTP access).
!(httpss://placehold.co/400x300/1e1e1e/c0c0c0?text=Display+Control+Module.jpg)
2. Wireless Transmitter Module
A standalone, battery-powered sensor package that wirelessly sends data to the main display.
Controller: ESP32
Sensors:
HC-SR04 Ultrasonic Distance Sensor
DHT11 Temperature & Humidity Sensor
Power: 18650 Li-ion battery with a BMS protection/charging board (via USB-C).
Wireless: ESP-NOW (to transmit sensor data).
!(httpss://placehold.co/400x300/1e1e1e/c0c0c0?text=Transmitter+Module.jpg)
Application & Software Deep Dive
Each of the 6 applications is a self-contained .ino file, called from the main dashboard loop.
1. Generative Art Engine
Animations are calculated by storing two 32x32 buffers (current frame and next frame). A kernel is moved over the entire grid, updating each pixel's state based on its neighbors.
Conway's Game of Life: Implements the classic ruleset for cell life and death.
Organic Shapes: Uses mean-shift clustering; a pixel's state is determined by whether the count of its active neighbors crosses a threshold.
Color: A "color wheel" of 255 hues is pre-calculated (HSV to 16-bit RGB) and stored for efficient access during animations.
2. WiFi-Connected Clock (NTP Sync)
The app receives time data from a Network Time Protocol (NTP) server. This is unpacked into seconds, minutes, and hours, which are combined into a single float to calculate the precise angle of the clock hands. Trigonometry is used to draw the hands over a custom clock-face bitmap. The rotary encoder can be used to add a manual time offset.
3. Snake Game with Joystick Control
Every segment of the snake is stored in a global array (to prevent stack overflow on the ESP32).
Input: Reads analog voltages from the joystick's potentiometers to determine the direction of travel.
Logic: The head's new position is calculated. The game checks for collisions against the wall or the snake's own body.
Update: Every segment is shifted one index down the array.
Food: If the head collides with the food, the score increases, and new food is randomly placed (with checks to ensure it doesn't spawn on the snake).
4. Color Sensor (Rubik's Cube Detection)
To get accurate readings, the raw R, G, and B values from the sensor are normalized by dividing each channel by the total light output (the 'C' channel). These normalized values are then compared to a set of pre-calibrated colors. The 3D Euclidean distance between the reading and the calibrated colors is calculated, and a match is declared if the distance is below a set threshold.
5. Weather Display (OpenWeatherMap API)
This app connects to the OpenWeatherMap API and parses the returned JSON payload to extract the current temperature and weather-condition code. The app displays the time and temperature, and then renders the correct weather icon from a large "sprite sheet" bitmap by shifting the bitmap to the correct x-position based on the weather code.
6. Wireless Sensor Dashboard (ESP-NOW)
I chose ESP-NOW for its low-latency and efficiency. A struct is defined on both controllers to package the temperature, humidity, and distance data. The transmitter module collects this data and sends the struct (as individual bytes) to the display module, where a callback function receives it and updates global variables for display.
Hardware Design & Build Process
Panel Selection & Assembly
I chose 8x32 panels (instead of 16x16) for future customizability, as they can be re-arranged into other form factors (like a long 8x128 scroller). The panels were positioned in an alternating (zigzag) pattern to minimize signal wiring between them. They are secured with tape and reinforced with wood skewers for rigidity.
Power Delivery (60A System)
At full brightness (all pixels white), the 1024-pixel matrix can draw up to 60 amps at 5V (300W). The power delivery system was designed to handle this.
The stock wires on the panels were replaced with thicker gauge wire.
All power leads were combined and routed into a robust, high-current DC connector.
A 5V, 300W switching power supply is used.
Multiple lengths of speaker wire were bundled in parallel for the main power cable to minimize voltage drop over its long range.
Prototyping
All components are prototyped on breadboards. The display module features longer wires for ergonomic use of the joystick and color sensor, while the transmitter module is kept as compact as possible.
Code & File Structure
Dashboard_rev6/
Dashboard_rev6.ino: The main entry point. Handles library imports, WiFi connection, and the main UI loop for app selection.
Animation.ino, Clock.ino, Snake.ino, ColorSense.ino, Weather.ino, wireless.ino: Each file contains the complete code for one of the 6 applications.
.h files: (e.g., clockface.h, weathericons.h). These header files store the bitmap images as 16-bit integer arrays, which are loaded into program memory.
Dashboard_transmitter_rev6/
Dashboard_transmitter_rev6.ino: The complete firmware for the transmitter module. It connects to WiFi, establishes the ESP-NOW connection, polls the sensors, and transmits the data_message struct.


