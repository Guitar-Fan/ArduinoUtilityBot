#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Adafruit_7segment led_display1 = Adafruit_7segment();
Adafruit_7segment led_display2 = Adafruit_7segment();


const int buttonP3 = 8;
const int buttonP2 = 9;
const int buttonP1 = 10;
const int buzzer = 7;

int setupPlayer = 0;
int setupNumber = 0;
int currentPlayer = 0;
bool gameRunning = false, gamePaused = false, whiteWon = false, blackWon = false;
int player1Minutes = 0, player1Seconds = 0, player2Minutes = 0, player2Seconds = 0, increment = 0;
int clone1, clone1s, clone2, clone2s;
char player1Time[5] = "0000", player2Time[5] = "0000";
int centiCounter1 = 0, centiCounter2 = 0, centiBeepCounter = 0;
int whiteGames = 0, blackGames = 0;
bool gameStarted = true, beepOn = true, beeping = false, pauseMenu = false, casual = true;


bool buttonP1pressed = false, buttonP2pressed = false, buttonP3pressed = false;

// Preset Frequencies For Making COOL Ringtones
const int G3 = 196, Arl = 220, B3 = 247, C4 = 262, C45 = 278, D4 = 294, E4 = 330, F4 = 349, F45 = 372, G4 = 392, Aru = 440, B4 = 494, C5 = 523, D5 = 566;

// Blaze of Glory by Bon Jovi riff
int melody[] = {
  Aru, Aru, Aru, Aru, C5, D5, Aru, G4, G4, F4, F4, E4, F4, D4
};

float noteDurations[] = {
  2.4, 4.8, 2.88, 2.88, 2.88, 2.88, 1.9, 2.4, 4.8, 2.88, 2.88, 2.88, 2.88, 2.88
};

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
  lcd.print(" Ultimate: ");
  lcd.setCursor(0, 1);
  lcd.print(" Chess Clock ");
  delay(2000);
  lcd.clear();
  lcd.print("By: And He Sacks, ");
  lcd.setCursor(0, 1);
  delay(500);
  lcd.print("THE ROOK!!!");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Don't like Square Waves???");

  get_ans(beepOn);

// Competitive or Casual Prompt
  delay(200);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Playing Magnus");
  lcd.setCursor(0, 1);
  lcd.print(" or Martin?");

  get_ans(casual);
  bool checker2 = true;
  if (EEPROM.read(7) != casual) {
	EEPROM.write(7, casual);
	checker2 = false;
  }

  if (checker2) {
	delay(200);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Use previous");
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
  lcd.print("Use previous");
  lcd.setCursor(0, 1);
  lcd.print("score?");
  
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
    while (gameStarted) {
      if (digitalRead(buttonP1) == HIGH) {
        gameStarted = false;
        currentPlayer = 1;
        lcd.clear();
      }
    }
    
    if (digitalRead(buttonP1) == HIGH && currentPlayer != 1) {
	  if (casual) {
		player1Minutes = clone1;
		player1Seconds = clone1s;
	  } else {
	    player1Seconds += increment;
	    while (player1Seconds >= 60) {
	      player1Minutes++;
	      player1Seconds -= 60;
	    }
	  }
      currentPlayer = 1;
      led_display1.drawColon(true);
      led_display1.writeDisplay();
      if (beepOn) {
        tone(buzzer, 523);
        delay(100);
        noTone(buzzer);
        beeping = false;
        centiBeepCounter = 0;
      }
    } else if (digitalRead(buttonP2) == HIGH && currentPlayer != 0) {
	  if (casual) {
		player2Minutes = clone2;
		player2Seconds = clone2s;
	  } else {
	    player2Seconds += increment;
	      while (player2Seconds >= 60) {
	        player2Minutes++;
	        player2Seconds -= 60;
	      }
	  }
      currentPlayer = 0;
      led_display2.drawColon(true);
      led_display2.writeDisplay();
      if (beepOn) {
        tone(buzzer, 523);
        delay(100);
        noTone(buzzer);
        beeping = false;
        centiBeepCounter = 0;
      }
    } else if (digitalRead(buttonP3) == HIGH) {
      buttonP3pressed = true;
    } else {
      buttonP1pressed = false;
      buttonP2pressed = false;
      buttonP3pressed = false;
    }

    advanceTime();

    if (whiteWon) {
      gamePaused = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Black timeout");
    } else if (blackWon) {
      gamePaused = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("White timeout");
    }

    while (gamePaused) {
      while (whiteWon || blackWon) {
        if (beepOn) {
          for (int thisNote = 0; thisNote < 16; thisNote++) {
    		int noteDuration = 1000 / noteDurations[thisNote];
    		tone(buzzer, melody[thisNote], noteDuration);
    		int pauseBetweenNotes = noteDuration * 1.30;
    		delay(pauseBetweenNotes);
    		noTone(buzzer);
  		  }
  		  delay(500);
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
    if (setupNumber < 1) {
      setupNumber++;
    } else if (setupPlayer < 1) {
      setupNumber = 0;
      setupPlayer++;
    } else if (setupPlayer == 1) {
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
    } else {
	  if (notPaused) {
	    startingGame();
	    if (increment != EEPROM.read(4)) EEPROM.write(4, increment);
	      
	    lcd.clear();
	    displayCurrentTime();
	  } else {
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
	      if (player1Minutes >= 15 && player1Minutes <= 25) player1Minutes += 5;
	      else if (player1Minutes == 30) player1Minutes += 10;
	      else if (player1Minutes >= 40 && player1Minutes < 50) player1Minutes += 5;
	      else if (player1Minutes >= 50) player1Minutes += 10;
	      else player1Minutes++;
		} else {
		  player1Minutes++;
		}
        if (player1Minutes > 120) player1Minutes = 120;
      } else if (setupNumber == 1) {
        player1Seconds++;
        if (player1Seconds >= 60) player1Seconds = 59; // If seconds are over 59, bring it back to 59
      }
    } else if (setupPlayer == 1) { // Player 2 (Black)'s time is being set
      if (setupNumber == 0) { // Minutes are being set
		if (notPaused) {
	      if (player2Minutes >= 15 && player2Minutes <= 25) player2Minutes += 5;
	      else if (player2Minutes == 30) player2Minutes += 10;
	      else if (player2Minutes >= 40 && player2Minutes < 50) player2Minutes += 5;
	      else if (player2Minutes >= 50) player2Minutes += 10;
	      else player2Minutes++;
		} else {
		  player2Minutes++;
		}
        if (player2Minutes > 120) player2Minutes = 120;
      } else if (setupNumber == 1) {
        player2Seconds++;
        if (player2Seconds >= 60) player2Seconds = 59; // If seconds are over 59, bring it back to 59
      }
    } else if (setupNumber == 2) {
	  if (notPaused) {
	    if (increment >= 15) increment += 5;
	    else increment++;
	  } else {
		increment++;
	  }
      if (increment > 60) increment = 60;
    }
  }
  if (buttonP1pressed) {
    if (setupPlayer == 0) {
      if (setupNumber == 0) {
		if (notPaused) {
	      if (player1Minutes >= 20 && player1Minutes <= 30) player1Minutes -= 5;
	      else if (player1Minutes == 40) player1Minutes -= 10;
	      else if (player1Minutes == 45 || player1Minutes == 50) player1Minutes -= 5;
	      else if (player1Minutes >= 60) player1Minutes -= 10;
	      else player1Minutes--;
		} else {
		  player1Minutes--;
		}
        if (player1Minutes < 0) player1Minutes = 0;
      } else if (setupNumber == 1) {
        player1Seconds--;
        if (player1Seconds < 0) player1Seconds = 0; // If seconds are negative, bring it back to 0
      }
    } else if (setupPlayer == 1) { // Player 2 (Black)'s time is being set
      if (setupNumber == 0) { // Minutes are being set
		if (notPaused) {
	      if (player2Minutes >= 20 && player2Minutes <= 30) player2Minutes -= 5;
	      else if (player2Minutes == 40) player2Minutes -= 10;
	      else if (player2Minutes == 45 || player2Minutes == 50) player2Minutes -= 5;
	      else if (player2Minutes >= 60) player2Minutes -= 10;
	      else player2Minutes--;
		} else {
		  player2Minutes--;
		}
        if (player2Minutes < 0) player2Minutes = 0;
      } else if (setupNumber == 1) {
        player2Seconds--;
        if (player2Seconds < 0) player2Seconds = 0; // If seconds are negative, bring it back to 0
      }
    } else if (setupNumber == 2) {
	  if (notPaused) {
	    if (increment >= 20) increment -= 5;
	    else increment--;
	  } else {
		increment--;
	  }
      if (increment < 0) increment = 0;
    }
  }
}

void updateScreen() {
  
  const char* labels[] = {"Minutes: ", "Seconds: ", "Increment: "};
  const char* player[] = {"Player 1: ", "Player 2: ", "Both: "};

  // Display appropriate labels
  lcd.setCursor(0,0);
  lcd.print(player[setupPlayer]);
  lcd.setCursor(0,1);
  lcd.print(labels[setupNumber]);

  if (setupPlayer == 0) {
    
    led_display1.clear();
    setTime(player1Minutes, true, true);
    setTime(player1Seconds, true, false);
    led_display1.println(player1Time);
    led_display1.drawColon(true);
    led_display1.writeDisplay();
  } else if (setupPlayer == 1) {
    
    led_display2.clear();
    setTime(player2Minutes, false, true);
    setTime(player2Seconds, false, false);
    led_display2.println(player2Time);
    led_display2.drawColon(true);
    led_display2.writeDisplay();
  } else {
    
    led_display1.clear();
    led_display2.clear();
    led_display1.println(increment);
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
    return; // Return execution to loop()
  }

  // Update timer based on whose turn it is
  if (currentPlayer == 0) { // White's turn
    if (centiCounter1 < 10) { // As this function only runs every 0.01s, this centisecond counter will increment at the correct time
      centiCounter1++;
    } else if (player1Seconds > 0) { // If there are seconds left to reduce
      centiCounter1 = 0;
      player1Seconds--;
    } else if (player1Minutes > 0) { // If there are minutes left to reduce
      centiCounter1 = 0;
      player1Minutes--;
      player1Seconds = 59;
    } else {
      blackWon = true;
      blackGames++;
      if (whiteGames != EEPROM.read(5)) EEPROM.write(5, whiteGames);
      if (blackGames != EEPROM.read(6)) EEPROM.write(6, blackGames);
    }
  } else if (currentPlayer == 1) {
    if (centiCounter2 < 10) {
      centiCounter2++;
    } else if (player2Seconds > 0) {
      centiCounter1 = 0;
      player1Seconds--;
    } else if (player1Minutes > 0) {
      centiCounter2 = 0;
      player2Minutes--;
      player2Seconds = 59;
    } else {
      whiteWon = true;
      whiteGames++;
      if (whiteGames != EEPROM.read(5)) EEPROM.write(5, whiteGames);
      if (blackGames != EEPROM.read(6)) EEPROM.write(6, blackGames);
    }
  }

  displayCurrentTime();
}

void displayCurrentTime() {
  
  if (beeping && centiBeepCounter != 10) {
    centiBeepCounter++;
  } else if (beeping && centiBeepCounter == 10) {
    noTone(buzzer);
    beeping = false;
  }               
  if (((player1Minutes == 1 && player1Seconds == 0) && currentPlayer == 0) || ((player2Minutes == 1 && player2Seconds == 0) && currentPlayer == 1)) {
    if (beepOn && (!gameStarted)) {
      tone(buzzer, 523);
      centiBeepCounter = 0;
      beeping = true;
    }
  } else if (((player1Minutes == 0 && player1Seconds == 10) && currentPlayer == 0) || ((player2Minutes == 0 && player2Seconds == 10) && currentPlayer == 1)) {
    if (beepOn && (!gameStarted)) {
      tone(buzzer, 523);
      centiBeepCounter = 0;
      beeping = true;
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("<- W");
  lcd.setCursor(12, 0);
  lcd.print("B ->");
  lcd.setCursor(6, 0);
  lcd.print(whiteGames);
  lcd.print("- ");
  lcd.print(blackGames);

  if (!casual) {
    
    lcd.setCursor(5, 1);
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

    if (diffSeconds < 0 && diffMinutes > 0) {
      diffMinutes--;
      diffSeconds += 60;
    } else if (diffSeconds > 0 && diffMinutes < 0) {
      diffMinutes++;
      diffSeconds -= 60;
    }


    if ((diffMinutes > 0) || (diffMinutes == 0 && diffSeconds >= 0)) {
      lcd.print("+");
    } else if ((diffMinutes < 0) || (diffMinutes == 0 && diffSeconds < 0)) {
      lcd.print("-");
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
    lcd.print("Casual");
  }

  if (player1Minutes == 0 && player1Seconds <= 20) {
	lcd.setCursor(0, 1);
    lcd.print(".");
    lcd.print(10-centiCounter1);
    lcd.setCursor(2, 1);
    lcd.print(" ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("   ");
  }
	if (player2Minutes == 0 && player2Seconds <= 20) {
	  lcd.setCursor(14, 1);
    lcd.print(".");
    lcd.print(10-centiCounter2);
  } else {
    lcd.setCursor(14, 1);
    lcd.print("  ");
  }

  // Player 1 (White) time display
  led_display1.clear();
  led_display2.clear();
  if (centiCounter1 < 5 && currentPlayer == 0) {
    led_display1.drawColon(false);
    led_display2.drawColon(true);
  } else {
    led_display1.drawColon(true);
  } 
  if (centiCounter2 < 5 && currentPlayer == 1) {
    led_display2.drawColon(false);
    led_display1.drawColon(true);
  } else {
    led_display2.drawColon(true);
  }
  setTime(player1Minutes, true, true);
  setTime(player1Seconds, true, false);
  led_display1.println(player1Time);
  led_display1.writeDisplay();
  
  // Player 2 (Black) time display
  setTime(player2Minutes, false, true);
  setTime(player2Seconds, false, false);
  led_display2.println(player2Time);
  led_display2.writeDisplay();
}

// Sets the arrays player1Time and player2Time to the appropriate numbers
void setTime(int timeSetting, bool player1, bool minutes) {
  // Initialize variables
  int firstVar, secondVar;

  // Checking to set minutes or seconds
  if (minutes) {
    firstVar = 0;
    secondVar = 1;
  } else {
    firstVar = 2;
    secondVar = 3;
  }

  // Adjust the arrays based on timeSetting
  if (player1) {
    if (timeSetting < 10) {
      player1Time[firstVar] = '0';
      player1Time[secondVar] = timeSetting + '0';
    } else if (timeSetting < 100) {
      player1Time[firstVar] = timeSetting / 10 + '0';
      player1Time[secondVar] = timeSetting % 10 + '0';
    } else {
      // Hexadecimal Display
      int temp = timeSetting / 10;
      if (temp == 10) {
        player1Time[firstVar] = 'A';
      } else if (temp == 11) {
        player1Time[firstVar] = 'B';
      } else if (temp == 12) {
        player1Time[firstVar] = 'C';
      }
      player1Time[secondVar] = timeSetting % 10 + '0';
    }
  } else {
    if (timeSetting < 10) {
      player2Time[firstVar] = '0';
      player2Time[secondVar] = timeSetting + '0';
    } else if (timeSetting < 100) {
      player2Time[firstVar] = timeSetting / 10 + '0';
      player2Time[secondVar] = timeSetting % 10 + '0';
    } else {
      // Hexadecimal Display
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

void menuPause() {
  if (!pauseMenu) {
    // Display "Game Paused" message
    lcd.setCursor(0, 0);
    lcd.print("Game Paused");
  }
  if ((digitalRead(buttonP1) == HIGH) && (!pauseMenu)) {
    pauseMenu = true;
    setupPlayer = 0;
    setupNumber = 0;
    lcd.clear();
    updateScreen();
    delay(200); // Debounce delay
  } else if ((digitalRead(buttonP3) == HIGH) && (!pauseMenu)) { // Pause button is pressed to unpause the game
  	gamePaused = false;
    buttonP3pressed = false; // Ensure that the game isn't paused again in the advanceTime() function
    lcd.clear();
    delay(200); // Debounce delay
  } else if ((digitalRead(buttonP2) == HIGH) && (!pauseMenu)) {
	delay(200); // Debounce delay
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Who won?");
    delay(200); // Debounce delay
    bool checker3 = true;
	get_ans(checker3);
	if (!checker3) whiteGames++;
	else blackGames++;
    if (whiteGames != EEPROM.read(5)) EEPROM.write(5, whiteGames);
    if (blackGames != EEPROM.read(6)) EEPROM.write(6, blackGames);
		
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
      lcd.clear();
      displayCurrentTime();
	} else {
	  gameRunning = false;
      player1Minutes = 0;
      player1Seconds = 0;
      player2Minutes = 0;
      player2Seconds = 0;
      gamePaused = false;
      gameStarted = true;
      setupPlayer = 0;
	  setupNumber = 0;
      lcd.clear();
      buttonP1pressed = false;
      buttonP2pressed = false;
      buttonP3pressed = false;
  	  // Clear Player 2 Screen
      led_display2.clear();	
      led_display2.println("0000");
      led_display2.drawColon(true);
      led_display2.writeDisplay();
	}
  
    delay(200);
  }
  
  if (pauseMenu) {
	checkButtons();
		
    updateScreen();
    
    editTime(false);
  }
}

// Repeated functions
void get_ans(bool& var) {
  bool checker = true;
  while (checker) {
    if (digitalRead(buttonP1) == HIGH) {
      checker = false;
      var = false;
    } else if (digitalRead(buttonP2) == HIGH) {
      checker = false;
    }
  }
}

void checkButtons() {
  // Also, thanks to this if-else statement, when multiple buttons are pressed, player 1 button has 1st priority, and
  // player 2 button has 2nd priority
  if (digitalRead(buttonP1) == HIGH) { // Decrement button is pressed
    buttonP1pressed = true;
    buttonP2pressed = false;
    buttonP3pressed = false;
  } else if (digitalRead(buttonP2) == HIGH) { // Increment button is pressed
    buttonP1pressed = false;
    buttonP2pressed = true;
    buttonP3pressed = false;
  } else if (digitalRead(buttonP3) == HIGH) { // SET button is pressed
    buttonP1pressed = false;
    buttonP2pressed = false;
    buttonP3pressed = true;
  } else { // No buttons are pressed
    buttonP1pressed = false;
    buttonP2pressed = false;
    buttonP3pressed = false;
  }
}

void startingGame() {
  gameRunning = true;
	      
  if (player1Minutes == 0 && player1Seconds == 0) { // Sets player 1's time to 10 minutes if it's left blank
	player1Minutes = 10;
  }
  if (player2Minutes == 0 && player2Seconds == 0) { // Sets player 2's time to player 1's if it's left blank
	player2Minutes = player1Minutes;
	player2Seconds = player1Seconds;
  }
	
  clone1 = player1Minutes;
  clone1s = player1Seconds;
  clone2 = player2Minutes;
  clone2s = player2Seconds;
		      
  if (player1Minutes != EEPROM.read(0)) EEPROM.write(0, player1Minutes);
  if (player1Seconds != EEPROM.read(1)) EEPROM.write(1, player1Seconds);
  if (player2Minutes != EEPROM.read(2)) EEPROM.write(2, player2Minutes);
  if (player2Seconds != EEPROM.read(3)) EEPROM.write(3, player2Seconds);
}