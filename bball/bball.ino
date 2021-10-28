// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>
#include <IRremote.h>
#include <pitches.h>

// Variables -----------------------------------------------------------------------------------------------
// Game control
const int buttonPin = 5; // temp variable to represent ball going in
const int remotePin = 4;
const int buzzerPin = 3;


// Game
int score = 0;
int startTime = 0;
int endTime = 0;
bool gameInProgress = false; // Used to control physical timer and keep track of game state


// Display
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Timer 
Servo timer;
int timerAngle = 180;
const int timerResetAngle = 180;



// Game functions -----------------------------------------------------------------------------------------------
void startGame() {
  // time that the game will last 
  int gameTime = 5;  // 30 sec game
  
  score = 0;
  startTime = millis() / 1000;
  endTime = startTime + gameTime;
  timerAngle = timerResetAngle;
  timer.write(timerResetAngle);

  gameInProgress = true;

  lcd.clear();
  lcd.print("Welcome to score attack!");
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(200);
  }

  lcd.clear();
  lcd.print("Begin in...");
  lcd.setCursor(0, 1);
  lcd.print("3");
  delay(1000);
  lcd.print("2");
  delay(1000);
  lcd.print("1");
  delay(1000);
  
  lcd.setCursor(0, 0);
  lcd.print("Score attack!");
  lcd.setCursor(0, 1);
  lcd.print("Score: 0");
  

  Serial.print("start: ");
  Serial.println(startTime);
  Serial.print("End: ");
  Serial.println(endTime);
}

void resetGame() {
  Serial.println("Resetting game");
  score = 0;
  startTime = 0;
  endTime = 0;
  gameInProgress = false;

  // reset timer physical timer
  timer.write(timerResetAngle);

  lcd.clear();
  lcd.home();
  lcd.print("Welcome! ");
  lcd.setCursor(0, 1);
  lcd.print("1-Score, 2-Time");
  
}

void endGame() {
  // Monitor end time and end game if game in progress
  if (gameInProgress) {
    int current_time = millis() / 1000;

    if (current_time >= endTime) {
        // Game over
        Serial.println("GAME OVER");
        Serial.print("Score: ");
        Serial.println(score);
        delay(5000);

        resetGame();
    }
  }
  
}


// Utilliy functions -----------------------------------------------------------------------------------------------
void spinTimer() {
   if (gameInProgress) {
      timerAngle -= 1;
      if (timerAngle == 0) {
          timerAngle = 180; 
      }
     
//     Serial.print(timerAngle);
     timer.write(timerAngle);
    
     // 180 degrees/ 60sec = 3 degrees/sec   = 1 degree every 0.33 seconds
     delay(50); 
   }
   
}

bool incremented = false;
int incrementedTime = 0;
float incrementDelay = 200;
void incrementScore() {
  if(millis() > incrementedTime + incrementDelay) {
    incremented = false;
  }
  
  if (!incremented) {
    score += 1;
    lcd.print("Score: ");
    lcd.print(score);  
    incremented = true;
    incrementedTime = millis();
    tone(buzzerPin, NOTE_C5, 200);
  }

}

void listenToRemoteInput() {
  /*
    Remote control mapping
    power button - reset
    1 - Start score attack
    2 - Start time attack
    pause/play - pause/play game
    
  */

  if (IrReceiver.decode()) {
    int command = IrReceiver.decodedIRData.command;
    Serial.print("command: ");
    Serial.println(command);
    if (command == 69) { // Power button pressed (reset game)
        resetGame();  
    }

    if(command == 12) { // 1 button pressed (start score attack)
        startGame();
    }

//    if(command == 24) { // 2 button pressed (start time attack) 
//        Serial.println("Start time attack");  
//        digitalWrite(buzzerPin, HIGH);
//    }
    
  }
  IrReceiver.resume();
}

// RUN FUNCTIONS --------------------------------------------------------------------------------
void setup() {
  // Serial logging
  Serial.begin(9600);

  // Initial text
  lcd.begin(16, 2);
  lcd.print("Welcome! ");
  lcd.setCursor(0, 1);
  lcd.print("1-Score, 2-Time");

  // Attach physical timer and send angle to 0
  timer.attach(6);
  timer.write(timerAngle);


  // Pins
  IrReceiver.begin(remotePin, ENABLE_LED_FEEDBACK);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
}


void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  

  // Ball goes in
  if (digitalRead(buttonPin) == LOW) {
     incrementScore();
  }

  // Remote input
  listenToRemoteInput();

  // Check for gameEnd
  endGame();
  

  // Move timer
  spinTimer();
}
