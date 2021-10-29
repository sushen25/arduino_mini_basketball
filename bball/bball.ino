// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>
#include <IRremote.h>
#include <pitches.h>
#include <SR04.h>

// Variables -----------------------------------------------------------------------------------------------
// Game control
const int remotePin = 5;
const int buzzerPin = 4;

// ultrasonic sensor
const int trigPin = 2;
const int echoPin = 3;
SR04 bucketSensor = SR04(echoPin, trigPin);

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
  timerAngle = timerResetAngle;
  timer.write(timerResetAngle);


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

  startTime = millis() / 1000;
  endTime = startTime + gameTime;
  gameInProgress = true;
  
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
        lcd.clear();
        lcd.home();
        lcd.setCursor(0, 0);
        lcd.print("Game Over!");
        lcd.setCursor(0, 1);
        lcd.print("You scored ");
        lcd.print(score);
        lcd.print(" points!");
        delay(1500);

        for (int positionCounter = 0; positionCounter < (21 + 2); positionCounter++) {
          // scroll one position left:
          lcd.scrollDisplayLeft();
          // wait a bit:
          delay(200);
        }

        delay(5000);

        resetGame();
    }
  }
  
}


// Utilliy functions -----------------------------------------------------------------------------------------------
bool spun = false;
unsigned long spunTime = 0;
unsigned long spunDelay = 333; // 180 degrees/ 60sec = 3 degrees/sec  = 1 degree every 0.33 seconds
void spinTimer() {
   if (gameInProgress) {
      unsigned long currentTime = millis();

      if (currentTime > spunTime + spunDelay) {
          spun = false;  
      }

      if(!spun) {
          timerAngle -= 1; // 1 degree
          if (timerAngle == 0) {
              timerAngle = 180; 
          }
         
          timer.write(timerAngle);

          spun = true;
          spunTime = currentTime;
      }
   }
   
}

bool incremented = false;
unsigned long incrementedTime = 0;
unsigned long incrementDelay = 200;
void incrementScore() {
  unsigned long currentTime = millis();
  if(currentTime > incrementedTime + incrementDelay) {
    incremented = false;
  }
  
  if (!incremented && gameInProgress) {
    score += 1;
    lcd.print("Score: ");
    lcd.print(score);  
    incremented = true;
    incrementedTime = currentTime;
    tone(buzzerPin, NOTE_C5, 200);
  }

}

void detectBucket() {
    if (gameInProgress) {
        long distance = bucketSensor.Distance();
        Serial.println(distance);
        if (distance < 15) {
            Serial.println("BUCKET");
            incrementScore();
        } 
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
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);

  // Remote input
  listenToRemoteInput();

  // bucket sensor
  detectBucket();

  // Check for gameEnd
  endGame();
  

  // Move timer
  spinTimer();
}
