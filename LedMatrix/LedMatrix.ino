#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LedControl.h>

// --- Configuration ---

// Ultrasonic Sensor Pins (as seen in the Wokwi diagram)
const int TRIG_PIN = 10;
const int ECHO_PIN = 9;

// LED Pins
const int RED_LED_PIN = 6;
const int YELLOW_LED_PIN = 7;
const int GREEN_LED_PIN = 8;

// LCD I2C Address
const int LCD_ADDR = 0x27;

// LED Matrix Pins (MAX7219)
const int MATRIX_DIN_PIN = 5;  // Data In
const int MATRIX_CS_PIN = 4;   // Chip Select
const int MATRIX_CLK_PIN = 3;  // Clock

// --- Game Settings ---
const int MIN_CONTROL_DIST = 5;   // Closest distance for control (cm)
const int MAX_CONTROL_DIST = 40;  // Farthest distance for control (cm)
const int OBSTACLE_HEIGHT = 4;    // Height of the obstacle in pixels

// --- Global Objects ---
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
LedControl lc = LedControl(MATRIX_DIN_PIN, MATRIX_CLK_PIN, MATRIX_CS_PIN, 1);

// --- Game State Variables ---
int score = 0;
int playerRow = 3;      // Player's vertical position (0-7)
int obstacleCol = 7;    // Obstacle's horizontal position (0-7)
int obstacleGapRow = 2; // Top row of the obstacle's gap
bool isGameOver = false;

unsigned long lastMoveTime = 0;
int moveInterval = 400; // Initial speed (ms per column)

// --- Main Program ---

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize LED Matrix
  lc.shutdown(0, false);      // Wake up the display
  lc.setIntensity(0, 8);      // Set brightness (0-15)
  lc.clearDisplay(0);

  // Set pin modes
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // Seed the random number generator
  randomSeed(analogRead(A2)); // Use an unconnected pin for a random seed

  resetGame();
}

void loop() {
  if (isGameOver) {
    // If the game is over, check for a reset condition
    // A hand very close to the sensor will restart the game
    if (measureDistanceCM() < 5 && measureDistanceCM() > 0) {
      resetGame();
    }
    return; // Stop the game loop
  }

  // --- Game Running ---
  handlePlayerInput();
  updateGameLogic();
  drawGame();
}


// --- Helper Functions ---

/**
 * @brief Resets the game to its starting state.
 */
void resetGame() {
  isGameOver = false;
  score = 0;
  playerRow = 3;
  obstacleCol = 7;
  moveInterval = 400;
  
  // Generate the first obstacle
  obstacleGapRow = random(0, 8 - OBSTACLE_HEIGHT);

  // Update displays and LEDs for a new game
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Matrix Mover!");
  lcd.setCursor(0, 1);
  lcd.print("Score: 0");

  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  delay(1000);
}


/**
 * @brief Reads sensor and updates the player's position.
 */
void handlePlayerInput() {
  long distance = measureDistanceCM();
  // Map the hand distance to a row on the matrix (0-7)
  // Note: 7, 0 is inverted because closer distance = lower row number (top)
  playerRow = map(distance, MIN_CONTROL_DIST, MAX_CONTROL_DIST, 7, 0);
  // Keep the player within the screen bounds
  playerRow = constrain(playerRow, 0, 7);
}


/**
 * @brief Moves obstacles, checks for collisions, and updates the score.
 */
void updateGameLogic() {
  // Move the obstacle at a speed determined by moveInterval
  if (millis() - lastMoveTime > moveInterval) {
    lastMoveTime = millis();
    obstacleCol--; // Move obstacle one step to the left

    // Check for collision
    if (obstacleCol == 0) { // Player's column
      bool passed = (playerRow >= obstacleGapRow && playerRow < obstacleGapRow + OBSTACLE_HEIGHT);
      if (!passed) {
        // Collision detected!
        isGameOver = true;
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Game Over!");
        lcd.setCursor(0, 1);
        lcd.print("Score: " + String(score));
        return;
      }
    }
    
    // Check if obstacle has passed the player
    if (obstacleCol < 0) {
      score++;
      obstacleCol = 7; // Reset obstacle to the right side
      obstacleGapRow = random(0, 8 - OBSTACLE_HEIGHT); // New random gap
      
      // Increase speed slightly every 3 points
      if (score > 0 && score % 3 == 0 && moveInterval > 150) {
          moveInterval -= 25;
      }
      
      // Update score on LCD
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(score) + "   ");
      
      // Flash yellow LED for success
      digitalWrite(YELLOW_LED_PIN, HIGH);
      delay(100);
      digitalWrite(YELLOW_LED_PIN, LOW);
    }
  }
}

/**
 * @brief Clears and redraws the player and obstacles on the LED matrix.
 */
void drawGame() {
  lc.clearDisplay(0);

  // Draw the player
  lc.setLed(0, 0, playerRow, true);

  // Draw the obstacle (two parts: top and bottom)
  for (int r = 0; r < 8; r++) {
    // If the row 'r' is NOT in the gap, draw part of the obstacle
    if (!(r >= obstacleGapRow && r < obstacleGapRow + OBSTACLE_HEIGHT)) {
      lc.setLed(0, obstacleCol, r, true);
    }
  }
}

/**
 * @brief Measures distance in centimeters.
 * @return Distance in cm, or -1 if out of range.
 */
long measureDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 25000); // 25ms timeout
  long distance = duration * 0.0343 / 2;
  return (distance > 0 && distance < 400) ? distance : -1;
}