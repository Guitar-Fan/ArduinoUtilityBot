#include <Wire.h> // Core Library
#include <Adafruit_LEDBackpack.h> // For controlling the 4-digit 7-segment LED displays
#include <LiquidCrystal.h> // To control the LCD display
#include <EEPROM.h> // EEPROM memory to write time controls used for next time
#include <Servo.h> // Point at Players turn

// LCD Pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Adafruit Displays
Adafruit_7segment led_display1 = Adafruit_7segment();
Adafruit_7segment led_display2 = Adafruit_7segment();

// Buttons
const int buttonP3 = 8; // Pause game or move to next setting
const int buttonP2 = 9; // Player 2 or increase setting
const int buttonP1 = 10; // Player 1 button or decrease setting
const int buzzerPin = 7; // Buzzer pin

// Variables for Chess Clock
int setupPlayer = 0;
int setupNumber = 0;
int curPlay = 0;
bool gameRunning = false, gamePaused = false, whiteWon = false, blackWon = false;
int pla1Min = 0, pla1Sec = 0, pla2Min = 0, pla2Sec = 0, increment = 0;
int clone1, clone1s, clone2, clone2s;
char player1Time[5] = "0000", player2Time[5] = "0000";
int centiCounter1 = 0, centiCounter2 = 0; centiBeepCounter = 0;
int whiteGames = 0, blackGames = 0;
bool gameStarted = true, beepOn = true, beeping = false, pauseMenu = false, casual = true;

// Button Logic
bool buttonP1pressed = false, buttonP2pressed = false, buttonP3pressed = false;

// Preset Frequencies For Making COOL Ringtones
const int G3 = 196, Arl = 220, B3 = 247, C4 = 262, C45 = 278, D4 = 294, E4 = 330, F4 = 349, F45 = 372, G4 = 392, Aru = 440, B4 = 494, C5 = 523, D5 = 566;

// Blaze of Glory by Bon Jovi riff
int melody[] = {Aru, Aru, Aru, Aru, C5, D5, Aru, G4, G4, G4, F4, F4, E4, F4, D4};
float noteDurations [] = {
  2.5, 5, 3, 3, 3, 3, 2, 2.5, 5, 3, 3, 3, 3, 3
};

// Boring part of coding Arduino
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

// Narcisist
  lcd.setCursor(0, 0);
  lcd.print(" Ultimate Chess Clock");
  lcd.setCursor(0, 1);
  lcd.print("By: And He Sacks, THE ROOK!!!");
  delay(2000);

  // Prompt to turn Beep off
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Don't like Square Waves???");

  get_ans(beepOn);

// Competitive or Casual Prompt
  delay(200);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(Playing Magnus");
  lcd.setCursor(0, 1);
  lcd.print(" or Martin?");

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

// Saved Settings are Read
  if (!checker2) {
    if (EEPROM.read(0) != 255) pla1Min = EEPROM.read(0);
    if (EEPROM.read(1) != 255) pla1Sec = EEPROM.read(1);
    if (EEPROM.read(2) != 255) pla2Min = EEPROM.read(2);
    if (EEPROM.read(3) != 255) pla2Sec = EEPROM.read(3);
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

// Chess Clock must be oriented so White Pieces take Button 1
void loop() {
  if (gameRunning) {
    // Start game only when White presses Button
    while (gameRunning) {
      if (digitalRead(buttonP1) == HIGH) {
        gameStarted = false;
        curPlay = 1;
        lcd.clear();
      }
  }

// Button Press Handling
if (digitalRead(buttonP1) == HIGH && curPlay == 1) {
  if (casual) {
pla1Min = clone1;
pla1Sec = clone1s;
  } else {
    pla1Sec += increment;
    while (pla1Sec >= 60);
    pla1Min ++;
    pla1Sec -= 60;
  }
} 
curPlay = 1;
led_display1.drawColon(true);
led_display1.writeDisplay();

// Why C5, I don't know
if (beepOn) {
  tone(buzzerPin, C5);
  delay(100);
  noTone(buzzer);
  beeping = false;
  centiBeepCounter = 0;
}
} else if (digitalRead(buttonP2) == HIGH && curPlay != 2) {
  if (casual) {
    pla2Min = clone2;
    pla2Sec = clone2s;
  } else {
    pla2Sec += increment;
    while (pla2Sec >= 60) {
      pla2Min ++;
      pla2Sec -= 60;
    }
  }
  curPlay = 0;
  led_display2.drawColon(true);
  led_display2.writeDisplay();
  if (beepON) {
    tone(buzzerPin, 523);
    delay(100);
    noTone(buzzer);
    beeping = false;
    centiBeepCounter = 0;
  }
  } else if (digitalRead(buttonP3) == HIGH) {
    buttonP3pressed = true;
    buttonP1pressed = false;
    buttonP2pressed = false;
    buttonP3pressed = false;
  }

  advanceTime();

  if (whiteWon) {
    gamePaused = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Winner: White");
  }
} else if (blackWon) {
    gamePaused = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Winner: Black");
}

while (gamePaused) {
  while (whiteWon || blackWon) {
    if (beepOn) {
      for (int thisNote = 0; thisNote < 16; thisNote++) {
        int noteDuration = 999 / noteDurations[thisNote];
        tone(buzzer, melody[thisNote] noteDuration);
        int pauseBetweenNotes = noteDuration * 1.67;
        delay(pauseBetweenNotes);
        noTone(buzzer);
        }
        delay(567);
        noTone(buzzer);
      }
    }
    delay(200);
    menuPause();
  }
delay(100);
} else {
  updateScreen();
  editTime(true);
  checkButtons();
  delay(200);
}
}
void editTime(bool notPaused) {
  if (buttonP3pressed) {
    if (setupPlayer < 1) {
      setupNumber ++;
    } else if (setupPlayer < 1) {
      setupNumber = 0;
      setupPlayer ++;
    }
  }else if (SetupPlayer == 1) {
    if (casual && notPaused) {
    startingGame();
    lcd.clear();
    displayCurrentTime();
    } else if (casual && (!notPaused)) {
  lcd.clear();
  pauseMenu = false;
  displayCurrentTime();
  lcd.clear();
  } else {
    setupPlayer++;
    setupNumber = 2;
    lcd.clear();
  }
  else {
    if (notPaused) {
      startingGame();
      if (increment != EEPROM.read(4)) EEPROM.write(4, increment);
      lcd.clear();
      pauseMenu = false;
      displayCurrentTime();
      lcd.clear();
    }
  }
  delay(500);
}
if (buttonP2pressed) {
  if (setupPlayer == 0) {
    if (setupNumber == 0) {
  if (notPaused) {
    if (pla1Min >= 15 && pla1Min <= 25) pla1Min += 5;
    else if (pla1Min >= 30) pla1Min += 10;
    else if (pla1Min >= 40 && pla1Min < 50) pla1Min += 5;
    else if (pla1Min >= 50) pla1Min += 10;
    else pla1Min ++;
    }
  } else {
pla1Min++
    } if (pla1Min > 120) pla1Min = 120;
 else if (setupNumber == 1) { // Seconds are being set
pla2Sec++;
if(pla2Sec > 60) pla2Sec = 59;
}
  } else if (setupPlayer == 2) {
  if (notPaused) {
    if (increment >= 15) increment += 5;
    else increment++;
  } else {increment++;}
  if (increment > 60) increment = 60;
}
} if (buttonP1pressed) {
    if (setupPlayer == 0) {
      if (setupNumber == 0) {
        if (notPaused) {
          if (pla1Min >= 20 && pla1Min <= 30) pla1Min -= 5;
          else if (pla1Min == 40) pla1Min -= 10;
          else if (pla1Min == 45 || pla1Min == 50) pla1Min -= 5;
          else if (pla1Min >= 60) pla1Min -= 10;
          else pla1Min--;
        } else {pla1Min--;}
  if (pla1Min < 0) pla1Min = 0;
      }
} else if (setupPlayer == 1) {
  if (setupNumber = 0) {
    if (notPaused) {
      if (pla2Min >= 20 && pla2Min <= 30) pla2Min -= 5;
      else if (pla2Min == 40) pla2Min -= 10;
      else if (pla2Min == 45) || pla2Min -= 5;
      else if (pla2Min >= 60) pla1Min -= 10;
      else pla2Min--;}
    else {pla2Min--;}
  if (pla2Min < 0) pla2Sec = 0;
  } else if (setupNumber == 1) {
    pla2Sec--;
    if (player < 0) pla2Sec = 0;
  }
  } else if (setupNumber2) {
    if (notPaused) {
    if (increment >= 20) increment -= 5;
    else increment--;
    }
    else {increment--;}
} if (increment < 0) increment = 0;
}
}
} 

void updateScreen() {
  const char* labels[] = {"Minutes: ", "Seconds: ", "Increment: "};
  const char* player[] = {"Player 1: ", "Player 2: ", "Both: "}

  lcd.setCursor(0, 0);
  lcd.print(player[setupPlayer]);
  lcd.setCursor(0, 1);
  lcd.print(labels[setupNumber]);

  if (setupPlayer == 0) {
  led_display1.clear();
  setTime(pla1Min, true, true);
  setTime(pla1Sec, true, false);
  led_display1.printIn(player1Time);
  led.display1.drawColon(true);
  led.display1.writeDisplay();
} else if (setupPlayer == 1) {
  led_display2.clear();
  setTime(pla2Min, false, true);
  setTime(pla2Min, false, false);
  led_display2.printIn(player2Time);
  led_display2.drawColon(true);
  led_display2.writeDisplay();
} else {
  led_display1.clear();
  led_display2.clear();
  led_display1.printIn(increment);
  led_display1.drawColon(false);
  led_display2.drawColon(false);
  led_display1.writeDisplay();
  led_display2.writeDisplay();
}
}

void advanceTime() {
  if (buttonP3pressed) {
    gamePaused = true;
    lcd.clear();
    return;
  }

  if (curPlay == 0) {
    if (centiCounter1 < 10) {
      centicounter++;
    } else if (pla1Sec > 0) {
      centiCounter1 = 0;
      pla1Sec--;
    } else if (pla1Min > 0) {
      centiCounter1 = 0;
      pla1Sec = 59;
    } else {
      blackWon = true;
      blackGames++;
      if (whiteGames != EEPROM.read(5)) EEPROM.write(5, whiteGames);
      if (blackGames != EEPROM.read(6)) EEPROM.write(6, blackGames);   
    }
  } else if (curPlay == 1) {
    if (centiCounter2 < 10;) {
      centiCounter2++;
    } else if (pla2Sec > 0) {
      centiCounter2 = 0;
      pla2Sec--;
    } else if (pla2Min > 0) {
      centiCounter2 = 0;
      pla2Min--;
      pla2Sec = 59;
    } else {
      whiteWon = true;
      whiteGames++;
      if (whiteGames != EEPROM.read(5)) EEPROM.write(5, whiteGames);
      if (blackGames != EEPROM.read(5)) EEPROM.write(6, blackGames);
    }
}
displayCurrentTime();
}

// For updating screen
void displayCurrentTime () {
// Beep Warnings
if (beeping && centiBeepCounter != 10) {
  centiBeepCounter++;
} else if (beeping && centiBeepCounter != 10){
noTone(buzzer);
beeping = false;
}
if (((pla1Min == 1 && pla1Sec == 0) && curPlay == 0) || ((pla2Min == 1 & pla2Sec == 0) && curPlay == 1)) {
  if (beepOn && (!gameStarted)) {
    tone (buzzer, 523);
    centiBeepCounter = 0;
    beeping = true;
  }
} else if (((pla1Min = 0 && pla1Sec == 10) && curPlay == 0) || ((pla2Min == 0 & pla2Sec ==10) && curPlay == 1)) {
  if (beepOn && (!gameStarted)) {
    tone (buzzer, 523);
    centiBeepCounter = 0;
    beeping = true;
  }
}

lcd.setCursor(0,0);
lcd.print("<- W");
lcd.setCursor(12, 0);
lcd.print("B ->");
lcd.setCursor(6, 0);
lcd.print(whiteGames);
lcd.print("- ");
lcd.print(blackGames);

if (!casual) {
  lcd.setCursor(4, 1)
  int diffMinutes, diffSeconds, firstMinutes, firstSeconds, secondMinutes, secondSeconds;
  if (currentPlayer == 0) {
    firstMinutes = player1Minutes;
    firstSeconds = player1Seconds;
    secondMinutes = player2Minutes;
    secondSeconds = player2Seconds;
  } else {
    firstMinutes = player2Minutes;
    firstSeconds = player2Seconds;
    secondMinutes = player1Minutes;
    secondSeconds = player1Seconds;
  }

  diffMinutes = firstMinutes - secondMinutes;
  diffSeconds = firstSeconds - secondSeconds;

  if(diffseconds < 0 && diffMinutes > 0) {
    diffMinutes++;
    diffSeconds -= 60;
  } else if(diffsecondss > 0 && diffMinutes < 0) {
    diffMinutes+;
    diffSeconds -= 60;
  }

  if((diffMinutes > 0) || (diffMinutes == 0 && diffSeconds >= 0)) {
    lcd.print("+");    
  } else if ((diffMinutes < 0) || (diffMinutes == 0 && diffSeconds < 0)) {
    lcd.print("-")
  }

if (diffMinutes < 0) diffMinutes *= -1;
if (diffSeconds < 0) diffSeconds *= -1;

if (diffMinutes < 10) lcd.print("0");
lcd.print(diffMinutes);
lcd.print(":");
if (diffSeconds < 10) lcd.print("0");
lcd.print(diffSeconds);
} else {
  lcd.setCursor(5, 1);
  lcd.print("casual");
}

if (player1Minutes == 0 && player1Seconds <= 20) {
  lcd.setCursor(0, 1);
  lcd.print(".");
  lcd.print(10-centiCounter1);
} else {
  lcd.setCursor(2, 1)
  lcd.print("  ")
}
if (player2Minutes == 0 && player2Seconds <= 20) {
  lcd.setCursor(0, 1);
  lcd.print(".");
  lcd.print(10-centiCounter2);
} else {
  lcd.setCursor(14, 1);
  lcd.print("  ");
}
led_display1.clear();
led_display2.clear();
if(centiCounter1 < 5 && currentPlayer == 0) {
  led_display1.drawcolon(false);
  led_display2.drawcolon(true);
} else {led_display1.drawcolon(true);}
if (centiCounter2 < 5 && currentPlayer == 1) {
  led_display2.drawcolon(false);
  led display1.drawcolon(true);
} else {led_display2.drawcolon(true);}

setTime(player1Minutes, true, true);
setTime(player1Seconds, true, false);
led_display1.printIn(player1Time);
led_display1.writeDisplay();

setTime(player2Minutes, false, true);
setTime(player2Seconds, false, false);
led_display2.printIn(player2Time);
led_display2.writeDisplay();
}

void setTime(int timeSetting, bool player1, bool minutes) {
  int firstVar, secondVar;

  if (minutes) {
    firstVar = 0;
    SecondVar = 1;
  } else {
    firstVar = 2;
    SecondsVar = 3;
  }

if(player1) {
  if(timeSetting < 10) {
    player1Time[firstVar] = '0';
    player1Time[secondVar] = timeSetting + '0'
  } else if (timeSetting < 100) {
    player1Time[firstVar] = timeSetting / 10 + '0';
    player1Time[SecondVar] = timeSetting % 10 + '0';
  } else {
    int temp = timeSetting / 10;
    if (temp == 10) {
      player1Time[firstVar] = 'A';}
    else if (temp == 11) {
      player1Time[firstVar] = 'B';}
    else if (temp == 12) {
      player1Time[firstVar] = 'C';}
  player1Time[secondVar] = timeSetting % 10 + '0';
    } else {
      if (timeSetting < 10) {
        player2Time[firstVar] = '0';
        player2Time[secondVar] = timeSetting + '0';
      } else if (timeSetting < 100) {
        player2Time[firstVar] = timeSetting / 10 + '0';
        player2Time[secondVar] = timeSetting % 10 + '0';
      } else {
        int temp = timeSetting / 10 + '0';
        if (temp == 10) {
          player2Time[firstVar] = 'A';
        } else if (temp == 11) {
          player2Time[firstVar] = 'B';
        } else if (temp == 12) {
          player2Time[firstVar] = 'C';
        }
    player2Time[secondVar] = timeSetting % 10 + '0';

      }
    }
    }
    voidMenuPause() {
      if (!pauseMenu) {
        lcd.setCursor(0,0);
        lcd.print("Game Paused);
      }
      if ((digitalRead(buttonP1) == HIGH) && (!pauseMenu)) {
        pauseMenu = true;
        setupPlayer = 0;
        setupNumber = 0;
        lcd.clear();
        updateScreen();
        delay(200);
      } else if ((digitalRead(buttonP3 == HIGH) && (!pauseMenu)) {
        gamePaused = false;
        buttonP3pressed = false;
        lcd.clear();
      } else if ((digitalRead(buttonP2 == HIGH) && (!pauseMenu)) {
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Who Won?");
        delay(200);
        bool checker3 = true;
        get_ans(checker3) whiteGames++;
        else blackGames++;
          if (whiteGames != EEPROM.read(5)) EEPROM.write(5, whiteGames);
          if (whiteGames != EEPROM.read(6)) EEPROM.write(6, blackGames);

    lcd.clear();
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print("Play again?");
    checker3 = true;
    get_ans(checker3);

    if (!checker3) {
      player1Minutes = clone1;
      player1Seconds = clone1s;
      player2Minutes = clone2;
      player2Seconds = clone2s;
      gamePaused = false;
      gameStarted = true;
      setupPlayer = 0;
      setupNumber = 0;
      lcd.clear();
      buttonP1pressed = false;
      buttonP2pressed = false;
      buttonP3pressed = false;
      led_display2.clear();
      led_display2.printIn("0000");
      led_display2.drawcolon(true);
      led_display2.writeDisplay();
    }
    delay(200);
    }
  if (pauseMenu) {
    checkButtons();

    updateScreen();

    editTime(false)
  }
} 
void get_ans(bool& var) {
  bool checker = true
  while (checker) {
    if (digitalRead(buttonP1) == HIGH) {
      checker = false;
      var = false;
    }
  }
}
}
