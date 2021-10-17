// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>
#include <IRremote.h>

// Variables -----------------------------------------------------------------------------------------------
// Game control
int buttonPin = 5; // temp variable to represent ball going in
const int remotePin = 4;
IRrecv gameRemote(remotePin);
decode_results remoteInput;

// Game
int score = 0;
int startTime = 0;
int endTime = 0;
bool gameInProgress = false;


// Display
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Timer 
Servo timer;
int timerAngle = 180;



// Game functions -----------------------------------------------------------------------------------------------
void startGame() {
  // time that the game will last 
  int gameTime = 5;  // 30 sec game
  
  score = 0;
  startTime = millis() / 1000;
  endTime = startTime + gameTime;
  gameInProgress = true;

  // TODO - start timer

  Serial.print("start: ");
  Serial.println(startTime);
  Serial.print("End: ");
  Serial.println(endTime);
}

void resetGame() {
  score = 0;
  startTime = 0;
  endTime = 0;
  gameInProgress = false;

  // TODO - reset timer
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

        resetGame();
    }  
  }
  
}


// Utilliy functions -----------------------------------------------------------------------------------------------
void spinTimer() {
   timerAngle -= 1;
    if (timerAngle == 0) {
        timerAngle = 180; 
    }
   
   Serial.print(timerAngle);
   timer.write(timerAngle);

   // 180 degrees/ 60sec = 3 degrees/sec   = 1 degree every 0.33 seconds
   delay(333);
}

void incrementScore() {
  score += 1;
  lcd.print(score);

  return;                   

}

void listenToRemoteInput() {
  /*
    Remote control mapping
    power button - reset
    1 - Start score attack
    2 - Start time attack
    pause/play - pause/play game
    
  */
  
  if (gameRemote.decode(&remoteInput)) {
      Serial.println(remoteInput.value);
      if(remoteInput.value == 16724175) { // 1 button pressed (start score attack)
          startGame();
      }
  }
  gameRemote.resume();

}


void setup() {
  // Serial logging
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd.print("Score: ");

  // Attach physical timer and send angle to 0
  timer.attach(6);
  timer.write(timerAngle);

  // Setup remote
  gameRemote.enableIRIn();

  pinMode(buttonPin, INPUT_PULLUP);
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
//  spinTimer();
}
