#include <Wire.h> // For communicating with the 7-segment displays
#include <Adafruit_LEDBackpack.h> // For controlling the 4-digit 7-segment LED displays
#include <LiquidCrystal.h> // To control the LCD display
#include <EEPROM.h> // Use the EEPROM memory to write time controls used for next time

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Adafruit_7segment led_display1 = Adafruit_7segment();
Adafruit_7segment led_display2 = Adafruit_7segment();

const int buttonP3 = 8; // Pause game or move to next setting
const int buttonP2 = 9; // Player 2 or increase setting
const int buttonP1 = 10; // Player 1 button or decrease setting
const int buzzerPin = 7; // Buzzer pin

int setupPlayer = 0;
int setupNumber = 0;
int currentPlayer = 0;
bool gameRunning = false, gamePaused = false, whiteWon = false, blackWon = false;
int player1Minutes = 0, player1Seconds = 0, player2Minutes = 0, player2Seconds = 0, increment = 0;
int clone1, clone1s, clone2, clone2s;
char player1Time[5] = "0000", player2Time[5] = "0000";
int centiCounter1 = 0, centiCounter2 = 0; centiBeepCounter = 0;
int whiteGames = 0, blackGames = 0;
bool gameStarted = true, beepOn = true, beeping = false, pauseMenu = false, casual = true;

bool buttonP1pressed = false, buttonP2pressed = false, buttonP3pressed = false;

const int G3 = 196, Arl = 220, B3 = 247, C4 = 262, C45 = 278, D4 = 294, E4 = 330, F4 = 349, F45 = 372, G4 = 392, Aru = 440, B4 = 494, C5 = 523, D5 = 566;

void setup() {
  pinMode(buttonP3, INPUT);
  pinMode(buttonP2, INPUT);
  pinMode(buttonP1, INPUT);
  pinMode(buzzer, OUTPUT);

  led_display1.begin(112);
  led_display2.begin(113);
  led_display1.writeDisplay();
  led_display2.writeDisplay();

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print(" Ultimate Chess Clock");
  lcd.setCursor(0, 1);
  lcd.print("  By Sean Tran  ");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Turn beep off? ");

  get_ans(beepOn);
  
  delay(200);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(Competitive or");
  lcd.setCursor(0, 1);
  lcd.print("Casual?");

  get_ans(casual);
  bool checker2 = true;
  if (EEPROM.read(7) != casual) {
    EEPROM.write(7, casual);
    checker2 = false;
  }

if (!checker2) {
  delay(200);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Use Previous");
  lcd.setCursor(0, 1);
  lcd.print("time?");

  get_ans(checker2);

  if (!checker2) {
    if (EEPROM.read(0) != 255) player1Minutes = EEPROM.read(0);
    if (EEPROM.read(1) != 255) player1Seconds = EEPROM.read(1);
    if (EEPROM.read(2) != 255) player2Minutes = EEPROM.read(2);
    if (EEPROM.read(3) != 255) player2Seconds = EEPROM.read(3);
    if (EEPROM.read(4) != 255) increment = EEPROM.read(4);
    
    setupPlayer = 2;
    setupNumber = 2;
    buttonP3pressed = true;
  }
}

delay(200);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Use Previous");
lcd.setCursor(0, 1);
lcd.print("score?);

checker2 = true;
get_ans(checker2);

if (!checker2) {
  if (EEPROM.read(5) != 255) whiteGames = EEPROM.read(5);
  if (EEPROM.read(6) != 255) blackGames = EEPROM.read(6);
}

lcd.clear();
updateScreen();

}

void loop() {
  if (gameRunning) {
    while (gameRunning) {
      if (digitalRead(buttonP1) == HIGH) {
        gameStarted = false;
        currentPlayer = 1;
        lcd.clear();
      }
  }

if (digitalRead(buttonP1) == HIGH && currentPlayer == 1) {
  if (casual) {
player1Minutes = clone1;
player1Seconds = clone1s;
  } else {
    player1Seconds += increment;
    while (player1Seconds >= 60);
    player1Minutes ++;
    player1Seconds -= 60;
  }
} 
currentPlayer = 1;
led_display1.drawColon(true);
led_display1.writeDisplay();

if (beepOn) {
  tone(buzzerPin, C5);
  delay(100);
  noTone(buzzer);
  beeping = false;
  centiBeepCounter = 0;
}
}
