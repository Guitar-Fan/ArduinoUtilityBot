#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>

// Ultrasonic Sensor Pins (Using Analog pins as digital for convenience)
const int TRIG_PIN = A0;
const int ECHO_PIN = A1;

// LED Pins
const int RED_LED_PIN = 13;    // Closest distance
const int YELLOW_LED_PIN = 12; // Medium distance
const int GREEN_LED_PIN = 11;  // Farthest distance

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Distance Meter");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  // Set up sensor and LED pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // Initial state: turn all LEDs off
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  delay(2000); // Wait for a couple of seconds
}

// Function to measure distance in centimeters
long measureDistanceCM() {
  // Clear the Trig pin (ensure it's low)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Trigger a pulse of 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure the duration of the echo pulse (time for sound to travel)
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate distance: speed of sound is 340 m/s or 29.1 microseconds per cm (round trip)
  // Distance = (Duration / 2) / 29.1 
  long distanceCm = duration / 58; 

  // Limit the display distance to a reasonable range
  if (distanceCm > 400 || distanceCm < 2) {
    return -1; // -1 indicates out of range
  }
  
  return distanceCm;
}

void controlLEDs(long distance) {
  // Turn all LEDs off first
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  // Check distance ranges and turn on the corresponding LED
  if (distance > 0 && distance <= 10) {
    // Distance is 10cm or less (very close)
    digitalWrite(RED_LED_PIN, HIGH);
  } else if (distance > 10 && distance <= 30) {
    // Distance is between 10cm and 30cm (moderately close)
    digitalWrite(YELLOW_LED_PIN, HIGH);
  } else if (distance > 30 && distance <= 400) {
    // Distance is more than 30cm (far)
    digitalWrite(GREEN_LED_PIN, HIGH);
  }
}

void loop() {
  long distance = measureDistanceCM();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance:");

  if (distance == -1) {
    lcd.setCursor(0, 1);
    lcd.print("Out of Range");
    // Turn off all LEDs when out of range
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  } else {
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print(" cm");
    
    // Control LEDs based on the measured distance
    controlLEDs(distance);
  }
  
  delay(500); // Read and update every half second
}