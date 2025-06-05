#include <Adafruit_CircuitPlayground.h>

//  These are used to track game state
int gameStep = 0;  // 0=waiting, 1=flashing, 2=counting, 3=showing result
int level = 1;     // 1 or 2
int yellowFlashes = 0;
int purpleFlashes = 0;
int playerYellowCount = 0;
int playerPurpleCount = 0;

// Button tracking variables
bool leftPressed = 0;
bool rightPressed = 0;
bool bothPressed = 0;
int bothButtonTimer = 0;

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
}

void loop() {
  // Check which level we're on
  if (CircuitPlayground.slideSwitch()) {
    level = 2;  // Switch on = Level 2
  } else {
    level = 1;  // Switch off = Level 1
  }
  
  // Show which level we're on (only when waiting to start)
  if (gameStep == 0) {
    levelIndicator();
    checkForGameStart();
  }
  
  // run game steps
  if (gameStep == 1) {
    flashLEDs();
    gameStep = 2; // Go to counting step
    Serial.println("Now count the flashes!");
    Serial.println("Left button = Yellow, Right button = Purple (level 2)");
    Serial.println("Press both buttons when done counting to submit!");
    
  } else if (gameStep == 2) {
    counting();
    
  } else if (gameStep == 3) {
    showResult();
    delay(3000);
    endGame();
  }
  
  delay(50);
}

void checkForGameStart() {
  // Check if both buttons are being held
  if (CircuitPlayground.leftButton() && CircuitPlayground.rightButton()) {
    if (bothButtonTimer == 0) {
      bothButtonTimer = millis(); // Start timing
      Serial.println("Hold both buttons");
    } else if (millis() - bothButtonTimer >= 2000) {
      // Both buttons held for 2 seconds
      startGame();
      bothButtonTimer = 0;
    }
  } else {
    bothButtonTimer = 0; // Reset timer if buttons released
  }
}

void levelIndicator() {
  // Turn off all LEDs first
  CircuitPlayground.clearPixels();
  
  // Show level with first LED
  if (level == 1) {
    CircuitPlayground.setPixelColor(0, 255, 255, 0); // Yellow for level 1
    Serial.println("Level 1 - Count Yellow Flashes");
  } else {
    CircuitPlayground.setPixelColor(0, 128, 0, 128); // Purple for level 2
    Serial.println("Level 2 - Count Yellow AND Purple Flashes seperately");
  }
}

void startGame() {
  Serial.println("GAME STARTING!");
  gameStep = 1; // Flashing step
  
  // Reset counters
  yellowFlashes = 0;
  purpleFlashes = 0;
  playerYellowCount = 0;
  playerPurpleCount = 0;
  
  // Turn off all LEDs
  CircuitPlayground.clearPixels();
  delay(1000);
}

void flashLEDs() {
  Serial.println("Watch carefully!");
  
  if (level == 1) {
    // Level 1: Only yellow flashes, flash 10-20 times
    yellowFlashes = random(10, 21);
    Serial.print("Flashing yellow ");
    // Serial.print(yellowFlashes);
    // Serial.println(" times");
    
    for (int i = 0; i < yellowFlashes; i++) {
      // Turn all LEDs yellow
      for (int j = 0; j < 10; j++) {
        CircuitPlayground.setPixelColor(j, 255, 255, 0);
      }
      delay(random(200, 800)); // Random flash time
      
      // Turn off all LEDs
      CircuitPlayground.clearPixels();
      delay(random(300, 1000)); // Random pause time
    }
    
  } else {
    // Level 2: Yellow and purple, flash 15-30 times total
    int totalFlashes = random(15, 31);
    Serial.print("Flashing ");
    // Serial.print(totalFlashes);
    // Serial.println(" times total");
    
    for (int i = 0; i < totalFlashes; i++) {
      // Randomly pick yellow or purple
      if (random(2) == 0) {
        // Flash yellow
        yellowFlashes++;
        for (int j = 0; j < 10; j++) {
          CircuitPlayground.setPixelColor(j, 255, 255, 0);
        }
      } else {
        // Flash purple
        purpleFlashes++;
        for (int j = 0; j < 10; j++) {
          CircuitPlayground.setPixelColor(j, 128, 0, 128);
        }
      }
      
      delay(random(200, 800)); // Random flash time
      CircuitPlayground.clearPixels();
      delay(random(300, 1000)); // Random pause time
    }
    
    Serial.print("Correct answers - Yellow: ");
    Serial.print(yellowFlashes);
    Serial.print(", Purple: ");
    Serial.println(purpleFlashes);
  }
}

void counting() {
  // Turn off LEDs during counting
  CircuitPlayground.clearPixels();
  
  // Handle left button press (for yellow flash counting)
  if (CircuitPlayground.leftButton() && !leftPressed) {
    leftPressed = 1;
    playerYellowCount++;
    Serial.print("Yellow count: ");
    Serial.println(playerYellowCount);
  }
  if (!CircuitPlayground.leftButton()) {
    leftPressed = 0;
  }
  
  // right button press (purple flash count in level 2)
  if (CircuitPlayground.rightButton() && !rightPressed) {
    rightPressed = 1;
    if (level == 2) {
      playerPurpleCount++;
      Serial.print("Purple count: ");
      Serial.println(playerPurpleCount);
    }
  }
  if (!CircuitPlayground.rightButton()) {
    rightPressed = 0;
  }
  
  // for submitting answer
  if (CircuitPlayground.leftButton() && CircuitPlayground.rightButton() && !bothPressed) {
    bothPressed = 1;
    Serial.println("SUBMITTING ANSWER");
    gameStep = 3; //Result step
  }
  if (!CircuitPlayground.leftButton() || !CircuitPlayground.rightButton()) {
    bothPressed = 0;
  }
}

void showResult() {
  bool perfect = 0;
  int totalError = 0;
  
  if (level == 1) {
    // Level 1: Only check yellow count
    int error = abs(playerYellowCount - yellowFlashes);
    totalError = error;
    perfect = (error == 0);
    
    Serial.print("You counted: ");
    Serial.print(playerYellowCount);
    Serial.print(", Correct answer: ");
    Serial.println(yellowFlashes);
    
  } else {
    // Level 2: Check both colors
    int yellowError = abs(playerYellowCount - yellowFlashes);
    int purpleError = abs(playerPurpleCount - purpleFlashes);
    totalError = yellowError + purpleError;
    perfect = (yellowError == 0 && purpleError == 0);
    
    Serial.print("Your counts - Yellow: ");
    Serial.print(playerYellowCount);
    Serial.print(", Purple: ");
    Serial.println(playerPurpleCount);
    Serial.print("Correct answers - Yellow: ");
    Serial.print(yellowFlashes);
    Serial.print(", Purple: ");
    Serial.println(purpleFlashes);
  }
  
  // three scenarios for results
  if (perfect) {
    Serial.println("PERFECT!");
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, 0, 255, 0); // Green
    }
    playHappySound();
    
  } else if (totalError <= 2) {
    Serial.println("CLOSE! Try again!");
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, 0, 0, 255); // Blue
    }
    playSadSound();
    
  } else {
    Serial.println("WAY OFF! Try again!");
    for (int i = 0; i < 10; i++) {
      CircuitPlayground.setPixelColor(i, 255, 0, 0); // Red
    }
    playSadSound();
  }
}

void playHappySound() {
  CircuitPlayground.playTone(523, 200); 
  delay(250);
  CircuitPlayground.playTone(659, 200); 
  delay(250);
  CircuitPlayground.playTone(784, 200); 
  delay(250);
  CircuitPlayground.playTone(1047, 200); 
}

void playSadSound() {
  CircuitPlayground.playTone(200, 300);
  delay(400);
  CircuitPlayground.playTone(150, 300);
  delay(400);
  CircuitPlayground.playTone(100, 300);
}

void endGame() {
  Serial.println("GAME OVER");
  Serial.println("Hold BOTH buttons for 2 seconds to play again!");
  gameStep = 0; // Back to waiting
  CircuitPlayground.clearPixels();
}