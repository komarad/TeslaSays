/* ENIAROF 19
 *  
 *  Project: Tesla Says
 *  Project Team: Ksenia Voronova, Mikhail Panfilov, Gregoire Delzongle, Sergey Komardenkov
 *  When and Where: Moscow, Russia, Polytech Festival (21-22 May 2016)
 *                  Moscow, Russia, TEDxMoscow (28 May 2016)
 *  
 *  Version: 0.7 (May 28th, 2016)
 */
 
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <DFPlayer_Mini_Mp3_II.h>
#include <Servo.h>
#include <LiquidCrystal.h>


/* ===== CONSTANTS ======= */
const int BAUD = 9600;

/* LANGUAGE */
const int LANGUAGE_RUS = 0;
const int LANGUAGE_ENG = 1000;
const int LANGUAGE_FRA = 2000;

/* OUTPUT (MP3 shields) */
const int OUTPUT_BACK = 1;
const int OUTPUT_SIDES = 2;

/* MODES */
const int IDLE_MODE = 0;
const int INTRO_MODE = 1;
const int TUTORIAL_MODE = 2;
const int GAME_MODE = 3;

/* STATES */
const int WAITING_FOR_JACK = 0;
const int STATE_INTRO_START = 100;
const int STATE_INTRO_PLAYING_MESSAGE = 101;
const int STATE_INTRO_PLAYING_SHORT_MESSAGE = 102;

const int STATE_TUTORIAL_START = 200;
const int STATE_TUTORIAL_PLAYING_MESSAGE_1 = 201;
const int STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_1 = 202;
const int STATE_TUTORIAL_PLAYING_MESSAGE_2 = 211;
const int STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_2 = 212;
const int STATE_TUTORIAL_PLAYING_MESSAGE_3 = 221;
const int STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_3 = 222;
const int STATE_TUTORIAL_PLAYING_MESSAGE_4 = 231;
const int STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_4 = 232;
const int STATE_TUTORIAL_PLAYING_MESSAGE_5 = 241;
const int STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_5 = 242;
const int STATE_TUTORIAL_PLAYING_MESSAGE_6 = 251;
const int STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_6 = 252;
const int STATE_TUTORIAL_PLAYING_TUTORIAL_COMPLETED = 261;

const int STATE_GAME_START = 300;

/* AUDIO FILE CODES */
const int AUDIO_TESLA_INTRO_MESSAGE = 101; // 19500ms
const int AUDIO_TESLA_INTRO_SHORT_MESSAGE = 102; // 3500ms
const int AUDIO_TESLA_TUTORIAL_MESSAGE_1 = 201; // 5000ms
const int AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_1 = 202; // 3000ms
const int AUDIO_TESLA_TUTORIAL_MESSAGE_2 = 211; // 8500ms
const int AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_2 = 212; // 4000ms
const int AUDIO_TESLA_TUTORIAL_MESSAGE_3 = 221; // 6000ms
const int AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_3 = 222; // 3000ms
const int AUDIO_TESLA_TUTORIAL_BEAT_1 = 223; // 4000ms - 1000 + 1000 + 1000 intervals
const int AUDIO_TESLA_TUTORIAL_BEAT_2 = 224; // 3600ms - 500 + 1000 + 1000 intervals
const int AUDIO_TESLA_TUTORIAL_BEAT_3 = 225; // 3500ms - 1000 + 500 + 1000 intervals
const int AUDIO_TESLA_TUTORIAL_MESSAGE_4 = 231; // 5500ms
const int AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_4 = 232; // 2000ms
const int AUDIO_TESLA_TUTORIAL_MESSAGE_5 = 241; // 9000ms
const int AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_5 = 242; // 2500ms
const int AUDIO_TESLA_TUTORIAL_MESSAGE_6 = 251; // 7000ms
const int AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_6 = 252; // 2500ms
const int AUDIO_TESLA_TUTORIAL_COMPLETED = 261; // 7000ms


/* GLOBAL VARIABLES */
int score = 0;
int mode;
int state;
long lscm = 0; // Last State Change Millis
int language;


/* PIN CONSTANTS */
/*Display*/
LiquidCrystal lcd(12, 11, 39, 38, 37, 36);

/* Servos */
Servo servo_right;
Servo servo_left;
const int SERVO_RIGHT_PIN = 9;
const int SERVO_LEFT_PIN = 10;

/* MP3 */
boolean teslaPlaying = false;
boolean stereoPlaying = false;
const int OUTPUT_SIDES_ISPLAYING_PIN = 14;
const int OUTPUT_BACK_ISPLAYING_PIN = 29;
const int DELAY_FOR_ASSISTANCE = 10000;

/* Buttons*/
const int CATCH_ELECTRON_BTN_1_PIN = 3;
long catchElectronBtn1LastPush = 0;
const int CATCH_ELECTRON_BTN_2_PIN = 4;
long catchElectronBtn2LastPush = 0;
const int CATCH_ELECTRON_BTN_3_PIN = 5;
long catchElectronBtn3LastPush = 0;
const int LIGHT_BTN_PIN = 7;
long lightBtnLastPush = 0;
const int MESSAGE_BTN_PIN = 6;
long messageBtnLastPush = 0;
const int EAR_BTN_PIN = 8;
long earBtnLastPush = 0;

/* Jacks */
const int INTRO_JACK_PIN = 31;
long introJackLastPush = 0;
const int TUTORIAL_JACK_PIN = 33;
long tutorialJackLastPush = 0;
const int GAME_JACK_PIN = 35;
long gameJackLastPush = 0;

/* LEDs */
const int CATCH_E_LED1_PIN = 23;
const int CATCH_E_LED2_PIN = 24;
const int CATCH_E_LED3_PIN = 25;
const int CATCH_E_LED4_PIN = 26;
const int CATCH_E_LED5_PIN = 27;
const int VOLTMETER_RIGHT_LED1 = 41;
const int VOLTMETER_RIGHT_LED2 = 43;
const int VOLTMETER_RIGHT_LED3 = 45;
const int VOLTMETER_RIGHT_LED4 = 47;
const int VOLTMETER_RIGHT_LED5 = 49;
const int VOLTMETER_LEFT_LED1 = 40;
const int VOLTMETER_LEFT_LED2 = 42;
const int VOLTMETER_LEFT_LED3 = 44;
const int VOLTMETER_LEFT_LED4 = 46;
const int VOLTMETER_LEFT_LED5 = 48;
const int INSIDE_LED = 50;

/* Tilt sensors */
const int TILT_1_PIN = 13;
long tiltLastPush = 0;

/* Potentiometers */
const int KNOB_LEFT_PIN = A0;
const int KNOB_RIGHT_PIN = A1;


/* VARIABLES USED IN GAME ASSIGNMENTS: */
/* Knobs assignment variables */
int leftGoal = -1;
int rightGoal = -1;
long assignment_completed_millis = 0;
const int AMOUNT_MILLIS_ALLOW_SERVOS_ROTATE = 500;
boolean knobsCompleted = false;

/* Knocking assignment variables */
const int VARIATIONS_NUM = 3;
const int KNOCKS_NUM_MINUS_ONE = 3;
const int KNOCKS_MILLIS_ERROR_WINDOW = 400;
int knocks_millis_ranges[VARIATIONS_NUM][KNOCKS_NUM_MINUS_ONE] = {
 {100,100,100},
 {50,100,100},
 {100,50,100},
}; /* multiply value by 10*/
int rangeSelected = -1;
long lastPressedMillis = -1;
int clicksPassed = 0;
boolean assignmentPlayed = false;


/* Bee assignment variables */
int numJumps = 0;
long lastJumpDetected = 0;
const int REQUIRED_NUM_JUMPS = 3;

/* Ear assignment variables */
int previousEarValue = 0;

/* LED catch assignment variables */
int currentLittenLED_PIN = 0;
long timeSinceCurrentLED_ON = 0;
int numberSuccessfulAttempts = 0;
int numberSingleLEDAttempts = 0;
long lastButtonPressDetected = 0;
int lastButtonPressedForLED = 0;
const int NUMBER_LED_ATTEMPTS = 1;
const int GOAL_OF_CORRECT_PUSHES = 3;

/* Throwaway variables */
int pos = 1;
int buttonState = 0;
int direction = 1;
int last_light_btn_state = 0;
int last_message_btn_state = 0;
int last_ear_btn_state = 0;
int tilt_btn_state = 0;
int catch_one_btn_state = 0;
int catch_two_btn_state = 0;
int catch_three_btn_state = 0;
int last_intro_jack_state = 0;
int last_tutorial_jack_state = 0;
int last_game_jack_state = 0;
int last_tilt_1_state = 0;



void setup () {

  mode = IDLE_MODE;
  changeState(WAITING_FOR_JACK);

  /* MP3 INIT */
  Serial.begin (BAUD);
  mp3_set_serial (Serial);
  mp3_set_volume (30);
  delay (100); // delays are needed for proper setup procedure
  Serial1.begin(BAUD);
  mp3_II_set_serial (Serial1);
  mp3_II_set_volume (15);
  delay (100); // delays are needed for proper setup procedure
  language = LANGUAGE_RUS;
  pinMode(OUTPUT_SIDES_ISPLAYING_PIN, INPUT); // Serial
  pinMode(OUTPUT_BACK_ISPLAYING_PIN, INPUT); // Serial1
  
  /* Servo INIT */
  servo_right.attach(SERVO_RIGHT_PIN);
  servo_left.attach(SERVO_LEFT_PIN);

  /* Button INIT */
  pinMode(LIGHT_BTN_PIN, INPUT_PULLUP);
  pinMode(MESSAGE_BTN_PIN, INPUT_PULLUP);
  pinMode(CATCH_ELECTRON_BTN_1_PIN, INPUT_PULLUP);
  pinMode(CATCH_ELECTRON_BTN_2_PIN, INPUT_PULLUP);
  pinMode(CATCH_ELECTRON_BTN_3_PIN, INPUT_PULLUP);
  pinMode(EAR_BTN_PIN, INPUT_PULLUP);

  /* Jack INIT */
  pinMode(INTRO_JACK_PIN, INPUT_PULLUP);
  pinMode(TUTORIAL_JACK_PIN, INPUT_PULLUP);
  pinMode(GAME_JACK_PIN, INPUT_PULLUP);

  /* LED INIT */
  pinMode(CATCH_E_LED1_PIN, OUTPUT);
  pinMode(CATCH_E_LED2_PIN, OUTPUT);
  pinMode(CATCH_E_LED3_PIN, OUTPUT);
  pinMode(CATCH_E_LED4_PIN, OUTPUT);
  pinMode(CATCH_E_LED5_PIN, OUTPUT);
  pinMode(INSIDE_LED, OUTPUT);
  pinMode(VOLTMETER_RIGHT_LED1, OUTPUT);
  pinMode(VOLTMETER_RIGHT_LED2, OUTPUT);
  pinMode(VOLTMETER_RIGHT_LED3, OUTPUT);
  pinMode(VOLTMETER_RIGHT_LED4, OUTPUT);
  pinMode(VOLTMETER_RIGHT_LED5, OUTPUT);
  pinMode(VOLTMETER_LEFT_LED1, OUTPUT);
  pinMode(VOLTMETER_LEFT_LED2, OUTPUT);
  pinMode(VOLTMETER_LEFT_LED3, OUTPUT);
  pinMode(VOLTMETER_LEFT_LED4, OUTPUT);
  pinMode(VOLTMETER_LEFT_LED5, OUTPUT);

  /* Tilt INIT */
  pinMode(TILT_1_PIN, INPUT_PULLUP);

  /* Knob INIT */
  pinMode(KNOB_LEFT_PIN, INPUT);
  pinMode(KNOB_RIGHT_PIN, INPUT);
  
  /* LCD */
  // Устанавливаем количество символов в строке (16) и количство строк (2)
  lcd.begin(16, 2);
  lcd.print("TESLA SAYS...");

}

/* ========================= */
/*
 * reSetup() reset all the gobal variables to their original values
*/
/* ========================= */
void reSetup() {
  leftGoal = -1;
  rightGoal = -1;
  rangeSelected = -1;
  lastPressedMillis = -1;
  clicksPassed = 0;
  assignmentPlayed = false;
  numJumps = 0;
  lastJumpDetected = 0;
  previousEarValue = 0;
  currentLittenLED_PIN = 0;
  timeSinceCurrentLED_ON = 0;
  numberSuccessfulAttempts = 0;
  numberSingleLEDAttempts = 0;
  lastButtonPressDetected = 0;
  lastButtonPressedForLED = 0;
  assignment_completed_millis = 0;
  knobsCompleted = false;

  digitalWrite(VOLTMETER_LEFT_LED1, 0);
  digitalWrite(VOLTMETER_LEFT_LED2, 0);
  digitalWrite(VOLTMETER_LEFT_LED3, 0);
  digitalWrite(VOLTMETER_LEFT_LED4, 0);
  digitalWrite(VOLTMETER_LEFT_LED5, 0);
  digitalWrite(VOLTMETER_RIGHT_LED1, 0);
  digitalWrite(VOLTMETER_RIGHT_LED2, 0);
  digitalWrite(VOLTMETER_RIGHT_LED3, 0);
  digitalWrite(VOLTMETER_RIGHT_LED4, 0);
  digitalWrite(VOLTMETER_RIGHT_LED5, 0);
  digitalWrite(VOLTMETER_RIGHT_LED5, 0);
  digitalWrite(INSIDE_LED, 0);
  digitalWrite(CATCH_E_LED1_PIN, 0);
  digitalWrite(CATCH_E_LED2_PIN, 0);
  digitalWrite(CATCH_E_LED3_PIN, 0);
  digitalWrite(CATCH_E_LED4_PIN, 0);
  digitalWrite(CATCH_E_LED5_PIN, 0);

}


/* ============ LOOP ============ */
void loop () {
  operate();
  //test(); // use operate() for the experience and test() for testing the circuit
}


/* ============ MAIN SUPPORT METHODS ============ */
/* ========================= */
/*
 * check_mode() reads socket inputs and replies with the numebr of
 * the socket were jack is currently plugged or IDLE_MODE (0) if it is not plugged
*/
/* ========================= */
int check_mode() {
  int reading = 0;

  reading = digitalRead(INTRO_JACK_PIN);
  if(reading == LOW) {
    return INTRO_MODE;
  }

  reading = digitalRead(TUTORIAL_JACK_PIN);
  if(reading == LOW) {
    return TUTORIAL_MODE;
  }
  
  reading = digitalRead(GAME_JACK_PIN);
  if(reading == LOW) {
    return GAME_MODE;
  }

  if(mode != IDLE_MODE && reading == HIGH) {
    return mode; // once you left IDLE, you cannot come back to it. You can only "travel" between other modes
  }
  
  return IDLE_MODE;
}


/* ========================= */
/*
 * changeState(int) accepts code of a new state to change to and performs the task
*/
/* ========================= */
void changeState (int newState) {
  state = newState;
  lscm = millis();
                                        Serial.print("State Change to "); Serial.println(newState);
}


/* ========================= */
/*
 * playSound(int, int, boolean) accepts code of the audio file to play, output channel (for multiple MP3 shields),
 * and directive to either ignore or use language (needed for audio file codes which do not possess verbal audio)
*/
/* ========================= */
void playSound(int soundCode, int output, boolean useLanguage = true) {
  
  if(output == OUTPUT_BACK) {
    mp3_II_play(soundCode + (useLanguage ? language : 0));
    mp3_play(soundCode + (useLanguage ? language : 0));  // temp - output same stuff from everywhere
  } else if (output == OUTPUT_SIDES) {
    //mp3_play(soundCode + (useLanguage ? language : 0));
  }

  teslaPlaying = true; // throwaway
}


/* ========================= */
/*
 * readButton(int) reads button state at specified pin
*/
/* ========================= */
int readButton (int buttonPin, boolean invert = false) {
  if(!invert) {
    return digitalRead(buttonPin);
  } else if(digitalRead(buttonPin) == LOW) {
    return HIGH;
  } else {
    return LOW;
  }
}


/* ========================= */
/*
 * debounce(int) stores last time sensor was read true
*/
/* ========================= */
void debounce(int pinNumber) {

  if(pinNumber == CATCH_ELECTRON_BTN_1_PIN) {
    catchElectronBtn1LastPush = millis();
  } else if (pinNumber == CATCH_ELECTRON_BTN_2_PIN) {
    catchElectronBtn2LastPush = millis();
  } else if (pinNumber == CATCH_ELECTRON_BTN_3_PIN) {
    catchElectronBtn3LastPush = millis();
  } else if (pinNumber == LIGHT_BTN_PIN) {
    lightBtnLastPush = millis();
  } else if (pinNumber == MESSAGE_BTN_PIN) {
    messageBtnLastPush = millis();
  } else if (pinNumber == EAR_BTN_PIN) {
    earBtnLastPush = millis();
  } else if (pinNumber == INTRO_JACK_PIN) {
    introJackLastPush = millis();
  } else if (pinNumber == TUTORIAL_JACK_PIN) {
    tutorialJackLastPush = millis();
  } else if (pinNumber == GAME_JACK_PIN) {
    gameJackLastPush = millis();
  } else if (pinNumber == TILT_1_PIN) {
    tiltLastPush = millis();
  }

}

/* ========================= */
/*
 * debounce(int) tells if it is ok to count sensor's true value as valid
*/
/* ========================= */
boolean debounceOk(int pinNumber, int millisDiff = 125) {

  if(pinNumber == CATCH_ELECTRON_BTN_1_PIN) {
    return millis() - catchElectronBtn1LastPush > millisDiff;
  } else if (pinNumber == CATCH_ELECTRON_BTN_2_PIN) {
    return millis() - catchElectronBtn2LastPush > millisDiff;
  } else if (pinNumber == CATCH_ELECTRON_BTN_3_PIN) {
    return millis() - catchElectronBtn3LastPush > millisDiff;
  } else if (pinNumber == LIGHT_BTN_PIN) {
    return millis() - lightBtnLastPush > millisDiff;
  } else if (pinNumber == MESSAGE_BTN_PIN) {
    return millis() - messageBtnLastPush > millisDiff;
  } else if (pinNumber == EAR_BTN_PIN) {
    return millis() - earBtnLastPush > millisDiff;
  } else if (pinNumber == INTRO_JACK_PIN) {
    return millis() - introJackLastPush > millisDiff;
  } else if (pinNumber == TUTORIAL_JACK_PIN) {
    return millis() - tutorialJackLastPush > millisDiff;
  } else if (pinNumber == GAME_JACK_PIN) {
    return millis() - gameJackLastPush > millisDiff;
  } else if (pinNumber == TILT_1_PIN) {
    return millis() - tiltLastPush > millisDiff;
  }
  
}


/* ========================= */
/*
 * readKnob(int) reads knob position converted to game's mapping (from 0 to 4 incl.)
*/
/* ========================= */
int readKnob (int whichKnob) {
  return readKnobRaw(whichKnob) / 204; // 204 gives 5 possible values: 0,1,2,3, and 4
}

/* ========================= */
/*
 * readKnobRaw(int) reads raw value of a knob (potentiometer)
*/
/* ========================= */
int readKnobRaw (int whichKnob) {
  return analogRead(whichKnob);
}



/* ========================= */
/*
 * setupKnobsAssignment creates random reference values for the knob assignment,
 * lights up appropriate LEDs on the board, sets Servos
*/
/* ========================= */
void setupKnobsAssignment () {
    
  int left_Knob_Reading = readKnob(KNOB_LEFT_PIN);
  int right_Knob_Reading = readKnob(KNOB_RIGHT_PIN);

  leftGoal = left_Knob_Reading;
  while(leftGoal == left_Knob_Reading) {
    randomSeed(millis());
    leftGoal = random(0,4);
  }

  rightGoal = right_Knob_Reading;
  while(rightGoal == right_Knob_Reading) {
    randomSeed(millis());
    rightGoal = random(0,4);
  }

  if(leftGoal == 0) { // turn on left aim LED
    digitalWrite(VOLTMETER_LEFT_LED1, 1);
  } else if (leftGoal == 1) {
    digitalWrite(VOLTMETER_LEFT_LED2, 1);
  } else if (leftGoal == 2) {
    digitalWrite(VOLTMETER_LEFT_LED3, 1);
  } else if (leftGoal == 3) {
    digitalWrite(VOLTMETER_LEFT_LED4, 1);
  } else if (leftGoal == 4) {
    digitalWrite(VOLTMETER_LEFT_LED5, 1);
  }

  if(rightGoal == 0) { // turn on right aim LED
    digitalWrite(VOLTMETER_RIGHT_LED1, 1);
  } else if (rightGoal == 1) {
    digitalWrite(VOLTMETER_RIGHT_LED2, 1);
  } else if (rightGoal == 2) {
    digitalWrite(VOLTMETER_RIGHT_LED3, 1);
  } else if (rightGoal == 3) {
    digitalWrite(VOLTMETER_RIGHT_LED4, 1);
  } else if (rightGoal == 4) {
    digitalWrite(VOLTMETER_RIGHT_LED5, 1);
  }
  
  attachServos();

  moveServo(SERVO_LEFT_PIN, 12);
  moveServo(SERVO_RIGHT_PIN, 1);
  delay(500);
}


/* ========================= */
/*
 * checkIfKnobsAreOk checks if knobs are aimed correctly and assignment is completed
*/
/* ========================= */
boolean checkIfKnobsAreOk() {
  int leftServoPos = readKnobRaw(KNOB_LEFT_PIN);
  int rightServoPos = readKnobRaw(KNOB_RIGHT_PIN);
  int leftServoPosAdj = readKnob(KNOB_LEFT_PIN);
  int rightServoPosAdj = readKnob(KNOB_RIGHT_PIN);
  
  //moveServo(SERVO_LEFT_PIN, map(leftServoPos,0,1023,5,175)); // simple mapping doesn't work due to the non-linear behaviour of poteniometers
  //moveServo(SERVO_RIGHT_PIN, map(rightServoPos,0,1023,5,175));

  if(leftServoPos > 1000) {
    moveServo(SERVO_LEFT_PIN, 175);
    leftServoPosAdj = 0;
  } else if (leftServoPos > 850) {
    moveServo(SERVO_LEFT_PIN, 150);
    leftServoPosAdj = 1;
  } else if (leftServoPos > 525) {
    moveServo(SERVO_LEFT_PIN, 90);
    leftServoPosAdj = 2;
  } else if (leftServoPos > 200) {
    moveServo(SERVO_LEFT_PIN, 40);
    leftServoPosAdj = 3;
  } else {
    moveServo(SERVO_LEFT_PIN, 12);
    leftServoPosAdj = 4;
  }

  if(rightServoPos > 900) {
    moveServo(SERVO_RIGHT_PIN, 1);
    rightServoPosAdj = 4;
  } else if (rightServoPos > 600) {
    moveServo(SERVO_RIGHT_PIN, 20);
    rightServoPosAdj = 3;
  } else if (rightServoPos > 400) {
    moveServo(SERVO_RIGHT_PIN, 80);
    rightServoPosAdj = 2;
  } else if (rightServoPos > 100) {
    moveServo(SERVO_RIGHT_PIN, 130);
    rightServoPosAdj = 1;
  } else {
    moveServo(SERVO_RIGHT_PIN, 175);
    rightServoPosAdj = 0;
  }

  boolean testResult = leftServoPosAdj == leftGoal && rightServoPosAdj == rightGoal;
  if(testResult) {
    if(leftGoal == 0) { // turn off left aim LED
      digitalWrite(VOLTMETER_LEFT_LED1, 0);
    } else if (leftGoal == 1) {
      digitalWrite(VOLTMETER_LEFT_LED2, 0);
    } else if (leftGoal == 2) {
      digitalWrite(VOLTMETER_LEFT_LED3, 0);
    } else if (leftGoal == 3) {
      digitalWrite(VOLTMETER_LEFT_LED4, 0);
    } else if (leftGoal == 4) {
      digitalWrite(VOLTMETER_LEFT_LED5, 0);
    }
  
    if(rightGoal == 0) { // turn off right aim LED
      digitalWrite(VOLTMETER_RIGHT_LED1, 0);
    } else if (rightGoal == 1) {
      digitalWrite(VOLTMETER_RIGHT_LED2, 0);
    } else if (rightGoal == 2) {
      digitalWrite(VOLTMETER_RIGHT_LED3, 0);
    } else if (rightGoal == 3) {
      digitalWrite(VOLTMETER_RIGHT_LED4, 0);
    } else if (rightGoal == 4) {
      digitalWrite(VOLTMETER_RIGHT_LED5, 0);
    }

    if(knobsCompleted) {
      if(millis() - assignment_completed_millis < AMOUNT_MILLIS_ALLOW_SERVOS_ROTATE) {
        return false; // wait a bit more...
      } else {
        return true; // ok, we waited long enough for servos to move to their final position. Now we are done.
      }
      
    } else {
      knobsCompleted = true;
      assignment_completed_millis = millis();
      return false; // we let servos run for a while and return true from the above if clause
    }
  }

  return testResult;
}


/* ========================= */
/*
 * setupKnocksAssignment picks one of the three random knock sequences for player to then click-play on the button
*/
/* ========================= */
void setupKnocksAssignment() {
  randomSeed(millis());
  rangeSelected = 2;//random(0,3); // remoed random as in the Tutorial mode it is hard to assist people from outside without knowing which assignment is playing
}


/* ========================= */
/*
 * checkIfKnocking checks if user completed knock pattern
*/
/* ========================= */
boolean checkIfKnocking() {

  if(debounceOk(MESSAGE_BTN_PIN)) {
    boolean buttonPressed = readButton(MESSAGE_BTN_PIN, true) == HIGH;
  
    if(lastPressedMillis > 0 && millis() - lastPressedMillis > 250) { // to account for possible short hold of a button (long hold has to be detected as incorrect, hence it passes)
    
        if(buttonPressed) {
          debounce(MESSAGE_BTN_PIN);
    
  Serial.println("Ok > 250 and button pressed");
  Serial.println(millis() - lastPressedMillis);
          int requiredTimeRange = 10*knocks_millis_ranges[rangeSelected][clicksPassed];
          if(millis() - lastPressedMillis > requiredTimeRange - KNOCKS_MILLIS_ERROR_WINDOW && millis() - lastPressedMillis < requiredTimeRange + KNOCKS_MILLIS_ERROR_WINDOW) {
            clicksPassed++;
  Serial.print("clicksPassed = ");
  Serial.println(clicksPassed);

                                                        
            if(clicksPassed == KNOCKS_NUM_MINUS_ONE) {
              return true; // success
            }
          } else {
            clicksPassed = 0;
          }
          lastPressedMillis = millis();
        }
    } else if (lastPressedMillis < 0 && buttonPressed) {
      Serial.println("Ok first click");
      lastPressedMillis = millis();
    }
  }
  
  return false; // keep listening
}


/* ========================= */
/*
 * checkIfBeeIsGone checks if user is jumoing and jumoed enough times
*/
/* ========================= */
boolean checkIfBeeIsGone() {

  if(lastJumpDetected == 0 || debounceOk(TILT_1_PIN, 250)) {

      boolean inAir = readButton(TILT_1_PIN, false) == HIGH;
      if(inAir) {
        debounce(TILT_1_PIN);
        numJumps++;
        lastJumpDetected = millis();
        if(numJumps >= REQUIRED_NUM_JUMPS) {
          numJumps = 0;
          return true;
        }
      }
  }
  
  return false; // keep listening
}


/* ========================= */
/*
 * setupLEDCatching 
*/
/* ========================= */
void setupLEDCatching() {
  currentLittenLED_PIN = 0;
  timeSinceCurrentLED_ON = 0;
  numberSuccessfulAttempts = 0;
  lastButtonPressDetected = 0;
  lastButtonPressedForLED = 0;
}


/* ========================= */
/*
 * checkIfLEDCatchingDone 
*/
/* ========================= */
boolean checkIfLEDCatchingDone() {

  if(lastButtonPressedForLED > 0) {
    boolean buttonReading = readButton(lastButtonPressedForLED, true) == HIGH;
    if(debounceOk(lastButtonPressedForLED, 50) && buttonReading) { // if debounce time has passed and we have the same button pressed that was pressed just before...
      return false;
    } else if (!debounceOk(lastButtonPressedForLED, 50)) { // if debounce time hasn't passed
      return false;
    } else if (!buttonReading) { // if lsat pressed button is not pressed anymore - ok to continue
      lastButtonPressedForLED = 0;
    }
  }

  if(lastButtonPressDetected == 0 || millis() - lastButtonPressDetected > 1000) {
    digitalWrite(currentLittenLED_PIN, 1); // enough time passed since an assignment - we can now lit up that LED
  } else if (millis() - lastButtonPressDetected < 1000) { 
    return false;  // although we have a new assignment (we picked an LED to ask user to "click") we wait for 1 sec.
  }
  
  
  if(currentLittenLED_PIN != 0) { //if LED is already ON, check if correct button pressed

    int requiredButtonPin = 0;
    boolean isCorrectButtonPressed = false;
    boolean buttonPressed = false;
    
    if(currentLittenLED_PIN == CATCH_E_LED1_PIN) { // first
      requiredButtonPin = CATCH_ELECTRON_BTN_1_PIN;
    } else if(currentLittenLED_PIN == CATCH_E_LED2_PIN) { // second
      requiredButtonPin = CATCH_ELECTRON_BTN_2_PIN;
    } else if(currentLittenLED_PIN == CATCH_E_LED3_PIN) { // third
      requiredButtonPin = CATCH_ELECTRON_BTN_3_PIN;
    }

    // if button is pressed, record attempt
    if(debounceOk(CATCH_ELECTRON_BTN_1_PIN, 50) && readButton(CATCH_ELECTRON_BTN_1_PIN, true) == HIGH) {
      buttonPressed = true;
      lastButtonPressDetected = millis();
      lastButtonPressedForLED = CATCH_ELECTRON_BTN_1_PIN;
      debounce(CATCH_ELECTRON_BTN_1_PIN);
      if(requiredButtonPin == CATCH_ELECTRON_BTN_1_PIN) {
        isCorrectButtonPressed = true;
      } else {
        isCorrectButtonPressed = false;
      }
    }

    // if button is pressed, record attempt
    if(debounceOk(CATCH_ELECTRON_BTN_2_PIN, 50) && readButton(CATCH_ELECTRON_BTN_2_PIN, true) == HIGH) {
      buttonPressed = true;
      lastButtonPressDetected = millis();
      lastButtonPressedForLED = CATCH_ELECTRON_BTN_2_PIN;
      debounce(CATCH_ELECTRON_BTN_2_PIN);
      if(requiredButtonPin == CATCH_ELECTRON_BTN_2_PIN) {
        isCorrectButtonPressed = true;
      } else {
        isCorrectButtonPressed = false;
      }
    }

    // if button is pressed, record attempt
    if(debounceOk(CATCH_ELECTRON_BTN_3_PIN, 50) && readButton(CATCH_ELECTRON_BTN_3_PIN, true) == HIGH) {
      buttonPressed = true;
      lastButtonPressDetected = millis();
      lastButtonPressedForLED = CATCH_ELECTRON_BTN_3_PIN;
      debounce(CATCH_ELECTRON_BTN_3_PIN);
      if(requiredButtonPin == CATCH_ELECTRON_BTN_3_PIN) {
        isCorrectButtonPressed = true;
      } else {
        isCorrectButtonPressed = false;
      }
    }

    if(buttonPressed && isCorrectButtonPressed) { // if correct button pressed, check if game is over (enough successful pushes)
        numberSuccessfulAttempts++;
        digitalWrite(currentLittenLED_PIN, 0);
        lastButtonPressDetected = millis();

        if(numberSuccessfulAttempts >= GOAL_OF_CORRECT_PUSHES) { // if game is over, turn off all LEDs and return "true"
            return true;
        } else { // if game is not over, pick a new LED to light up (return "false")
            randomSeed(millis());
            currentLittenLED_PIN = random(23,26);
            return false;
        }

        numberSingleLEDAttempts = 0;
        
        
    } else if (buttonPressed) {  // if incorrect button pressed, check if any more attempts left
      numberSingleLEDAttempts++;
      if(numberSingleLEDAttempts == NUMBER_LED_ATTEMPTS) {  // if there are no attempts left, set succefful count to 0 and pick a new LED to light up (return "false")
        numberSuccessfulAttempts = 0;
        numberSingleLEDAttempts = 0;
        digitalWrite(currentLittenLED_PIN, 0);
        randomSeed(millis());
        currentLittenLED_PIN = random(23,26);
        return false;
        
      } else { // if there are left attempts, keep listening (return "false")
        return false;
      }
    }

    return false;

  } else {  //if LED is off (first time we get here)
     randomSeed(millis());
     currentLittenLED_PIN = random(23,26);
     return false;
  }

}


/* ========================= */
/*
 * moveServo(int, int) moves servo to a specified position. First argument is the servo pin, and second is the position.
*/
/* ========================= */
void moveServo(int servoPin, int pos) {
  if(servoPin == SERVO_RIGHT_PIN) {
    servo_right.write(pos); 
  } else if (servoPin == SERVO_LEFT_PIN) {
    servo_left.write(pos); 
  }
}



/* ========================= */
/*
 * operate() is the main method of the code that has all the state logic
 * and is supposed to be run in each iteration of the loop
*/
/* ========================= */
void operate() {
 
  int mode_reading = check_mode();

  if(mode != mode_reading) {
    mode = mode_reading;
    delay(1000); // halt everything for the jack to get inserted
                                            Serial.print("Switching mode to ");Serial.println(mode);
    // cleanup previous mode:
    // TODO:


    // prepare new mode:
    if(mode == INTRO_MODE) {
      changeState(STATE_INTRO_START);
    }

    if(mode == TUTORIAL_MODE) {
      changeState(STATE_TUTORIAL_START);
    }

    if(mode == GAME_MODE) {
      changeState(STATE_GAME_START);
    }
    
  }

  if(mode == INTRO_MODE) {

    switch(state) {
      
      /* ========================================*/
      case STATE_INTRO_START:
        reSetup();
        playSound (AUDIO_TESLA_INTRO_MESSAGE, OUTPUT_BACK);
        changeState (STATE_INTRO_PLAYING_MESSAGE);
        /* ========================================*/
      break;


      /* ========================================*/
      case STATE_INTRO_PLAYING_MESSAGE:
        if(millis() - lscm > 19500 + DELAY_FOR_ASSISTANCE*1.5) {
          playSound (AUDIO_TESLA_INTRO_SHORT_MESSAGE, OUTPUT_BACK);
          changeState (WAITING_FOR_JACK);
        }
        /* ========================================*/
      break;


      /* ========================================*/
      case WAITING_FOR_JACK:
        if(millis() - lscm > 19500 + DELAY_FOR_ASSISTANCE*1.5) {
          playSound (AUDIO_TESLA_INTRO_SHORT_MESSAGE, OUTPUT_BACK);
          changeState (STATE_INTRO_PLAYING_MESSAGE);
        }
        /* ========================================*/
      break;

    }



  } else if (mode == TUTORIAL_MODE) {

    boolean isFirstTime = true;
    
    switch(state) {
      
      /* ========================================*/
      case STATE_TUTORIAL_START:
        playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_1, OUTPUT_BACK);
        changeState (STATE_TUTORIAL_PLAYING_MESSAGE_1);
        /* ========================================*/
      break;


       /* ========================================*/
      case STATE_TUTORIAL_PLAYING_MESSAGE_1:
        if(debounceOk(LIGHT_BTN_PIN) && readButton(LIGHT_BTN_PIN, true) == HIGH) {
          digitalWrite(INSIDE_LED, 1);
          debounce(LIGHT_BTN_PIN);
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_2, OUTPUT_BACK);
//delay(3000); // here we need a delay to let the message end due to the servos
          setupKnobsAssignment();
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_2);
        } else {
          if(millis() - lscm > 5000 + DELAY_FOR_ASSISTANCE) {
            playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_1, OUTPUT_BACK);
            changeState (STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_1);
          }
        }
        /* ========================================*/
      break;


       /* ========================================*/
      case STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_1:
        if(debounceOk(LIGHT_BTN_PIN) && readButton(LIGHT_BTN_PIN, true) == HIGH) {
          digitalWrite(INSIDE_LED, 1);
          debounce(LIGHT_BTN_PIN);
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_2, OUTPUT_BACK);
//delay(3000); // here we need a delay to let the message end due to the servos
          setupKnobsAssignment();
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_2);
        } else if(millis() - lscm > 5000 + DELAY_FOR_ASSISTANCE) {
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_1, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_1);
        }
        /* ========================================*/
      break;

        
      case STATE_TUTORIAL_PLAYING_MESSAGE_2:
        if(checkIfKnobsAreOk()) {
          detachServos();
          setupKnocksAssignment();
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_3, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_3);
        } else {
          if(millis() - lscm > 8500 + DELAY_FOR_ASSISTANCE) {
            isFirstTime = false;
            playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_2, OUTPUT_BACK);
            changeState (STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_2);
          }
        }
        /* ========================================*/
      break;


       /* ========================================*/
      case STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_2:
        if(checkIfKnobsAreOk()) {
          detachServos();
          setupKnocksAssignment();
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_3, OUTPUT_BACK);
          delay(7000);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_3);
        } else if(millis() - lscm > 8500 + DELAY_FOR_ASSISTANCE) {
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_2, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_2);
        }
        /* ========================================*/
      break;

      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_MESSAGE_3:
        if(checkIfKnocking()) {
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_4, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_4);
        }/* else if (!assignmentPlayed) {
          if(rangeSelected == 0) {
            playSound (AUDIO_TESLA_TUTORIAL_BEAT_1, OUTPUT_BACK);
          } else if(rangeSelected == 1) {
            playSound (AUDIO_TESLA_TUTORIAL_BEAT_2, OUTPUT_BACK);
          } else {
            playSound (AUDIO_TESLA_TUTORIAL_BEAT_3, OUTPUT_BACK);
          }
          assignmentPlayed = true;
          
        }*/ else if (millis() - lscm > (assignmentPlayed ? 6000 : 11000)) { // here we choose which of the three reference knocks
            assignmentPlayed = true;
            playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_3, OUTPUT_BACK);
/*delay(3500);
            if(rangeSelected == 0) {
              playSound (AUDIO_TESLA_TUTORIAL_BEAT_1, OUTPUT_BACK);
            } else if(rangeSelected == 1) {
              playSound (AUDIO_TESLA_TUTORIAL_BEAT_2, OUTPUT_BACK);
            } else {
              playSound (AUDIO_TESLA_TUTORIAL_BEAT_3, OUTPUT_BACK);
            }*/
            changeState (STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_3);
        }
        /* ========================================*/
      break;


      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_3:
        if(checkIfKnocking()) {
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_4, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_4);
        } else if (millis() - lscm > (assignmentPlayed ? 6000 : 11000)) {
          assignmentPlayed = true;
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_3, OUTPUT_BACK);
          /*delay(3500);
          if(rangeSelected == 0) {
            playSound (AUDIO_TESLA_TUTORIAL_BEAT_1, OUTPUT_BACK);
          } else if(rangeSelected == 1) {
            playSound (AUDIO_TESLA_TUTORIAL_BEAT_2, OUTPUT_BACK);
          } else {
            playSound (AUDIO_TESLA_TUTORIAL_BEAT_3, OUTPUT_BACK);
          }*/
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_3);
        }
        /* ========================================*/
      break;

      
      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_MESSAGE_4:
        if(checkIfBeeIsGone()) {
          setupLEDCatching();
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_5, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_5);
        } else if (millis() - lscm > 5500 + DELAY_FOR_ASSISTANCE) {
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_4, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_4);
        }
        /* ========================================*/
      break;


      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_4:
        if(checkIfBeeIsGone()) {
          setupLEDCatching();
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_5, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_5);
        } else if (millis() - lscm > 5500 + DELAY_FOR_ASSISTANCE) {
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_4, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_4);
        }
        /* ========================================*/
      break;


      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_MESSAGE_5:
        if(checkIfLEDCatchingDone()) {
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_6, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_6);
        } else if (millis() - lscm > 9000 + DELAY_FOR_ASSISTANCE) {
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_5, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_5);
        }
        /* ========================================*/
      break;


      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_5:
        if(checkIfLEDCatchingDone()) {
          playSound (AUDIO_TESLA_TUTORIAL_MESSAGE_6, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_6);
        } else if (millis() - lscm > 9000 + DELAY_FOR_ASSISTANCE) {
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_5, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_5);
        }
        /* ========================================*/
      break;


      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_MESSAGE_6:
        if(debounceOk(EAR_BTN_PIN) && readButton(EAR_BTN_PIN, true) == HIGH) {
          debounce(EAR_BTN_PIN);
          playSound (AUDIO_TESLA_TUTORIAL_COMPLETED, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_TUTORIAL_COMPLETED);
        } else if (millis() - lscm > 7000 + DELAY_FOR_ASSISTANCE) {
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_6, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_6);
        }
        /* ========================================*/
      break;


      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_SHORT_MESSAGE_6:
        if(debounceOk(EAR_BTN_PIN) && readButton(EAR_BTN_PIN, true) == HIGH) {
          debounce(EAR_BTN_PIN);
          playSound (AUDIO_TESLA_TUTORIAL_COMPLETED, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_TUTORIAL_COMPLETED);
        } else if (millis() - lscm > 7000 + DELAY_FOR_ASSISTANCE) {
          playSound (AUDIO_TESLA_TUTORIAL_SHORT_MESSAGE_6, OUTPUT_BACK);
          changeState (STATE_TUTORIAL_PLAYING_MESSAGE_6);
        }
        /* ========================================*/
      break;


      /* ========================================*/
      case STATE_TUTORIAL_PLAYING_TUTORIAL_COMPLETED:
        if(millis() - lscm > DELAY_FOR_ASSISTANCE) {
          changeState(WAITING_FOR_JACK);
        }
        /* ========================================*/
      break;
      
    }


  } else if (mode == GAME_MODE) {
    reSetup();
  }
  
}


/* ========================= */
void attachServos() {
  servo_right.attach(SERVO_RIGHT_PIN);
  servo_left.attach(SERVO_LEFT_PIN);
}

/* ========================= */
void detachServos() {
  servo_right.detach();
  servo_left.detach();
}



/* ========================= */
/*
 * test() is inherited from the previous versions of the code and could be run instead of operate to
 * test the circuit (all the components will react)
*/
/* ========================= */
void test () {

  /* Button tests */
  boolean buttonPressed = false;
  if(debounceOk(LIGHT_BTN_PIN) && readButton(LIGHT_BTN_PIN, true) == HIGH) {
      Serial.print("LIGHT BTN ");
      Serial.println(buttonState);
      debounce(LIGHT_BTN_PIN);
      buttonPressed = true;
  }
  if(debounceOk(MESSAGE_BTN_PIN) && readButton(MESSAGE_BTN_PIN, true) == HIGH) {
      Serial.print("MESSAGE BTN ");
      Serial.println(buttonState);
      debounce(MESSAGE_BTN_PIN);
      buttonPressed = true;
  }
  if(debounceOk(EAR_BTN_PIN) && readButton(EAR_BTN_PIN, true) == HIGH) {
      Serial.print("EAR BTN ");
      Serial.println(buttonState);
      debounce(EAR_BTN_PIN);
      buttonPressed = true;
  }
  if(debounceOk(TILT_1_PIN) && readButton(TILT_1_PIN, false) == HIGH) {
      Serial.print("TILT BTN ");
      Serial.println(buttonState);
      debounce(TILT_1_PIN);
      buttonPressed = true;
  }
  if(debounceOk(CATCH_ELECTRON_BTN_1_PIN) && readButton(CATCH_ELECTRON_BTN_1_PIN, true) == HIGH) {
      Serial.print("CATCH 1 BTN ");
      Serial.println(buttonState);
      debounce(CATCH_ELECTRON_BTN_1_PIN);
      buttonPressed = true;
  }
  if(debounceOk(CATCH_ELECTRON_BTN_2_PIN) && readButton(CATCH_ELECTRON_BTN_2_PIN, true) == HIGH) {
      Serial.print("CATCH 2 BTN ");
      Serial.println(buttonState);
      debounce(CATCH_ELECTRON_BTN_2_PIN);
      buttonPressed = true;
  }
  if(debounceOk(CATCH_ELECTRON_BTN_3_PIN) && readButton(CATCH_ELECTRON_BTN_3_PIN, true) == HIGH) {
      Serial.print("CATCH 3 BTN ");
      Serial.println(buttonState);
      debounce(CATCH_ELECTRON_BTN_3_PIN);
      buttonPressed = true;
  }

  /* LED tests */
  if(buttonPressed) {
    if(millis() % 2000 <= 1000) {
      digitalWrite(INSIDE_LED,1);
      digitalWrite(CATCH_E_LED1_PIN,1);
      digitalWrite(CATCH_E_LED2_PIN,1);
      digitalWrite(CATCH_E_LED3_PIN,1);
      digitalWrite(CATCH_E_LED4_PIN,0);
      digitalWrite(CATCH_E_LED5_PIN,0);
    } else {
      digitalWrite(INSIDE_LED,0);
      digitalWrite(CATCH_E_LED1_PIN,0);
      digitalWrite(CATCH_E_LED2_PIN,0);
      digitalWrite(CATCH_E_LED3_PIN,0);
      digitalWrite(CATCH_E_LED4_PIN,1);
      digitalWrite(CATCH_E_LED5_PIN,1);
    }

    digitalWrite(VOLTMETER_LEFT_LED1, 0);
    digitalWrite(VOLTMETER_RIGHT_LED1, 0);
    digitalWrite(VOLTMETER_LEFT_LED2, 0);
    digitalWrite(VOLTMETER_RIGHT_LED2, 0);
    digitalWrite(VOLTMETER_LEFT_LED3, 0);
    digitalWrite(VOLTMETER_RIGHT_LED3, 0);
    digitalWrite(VOLTMETER_LEFT_LED4, 0);
    digitalWrite(VOLTMETER_RIGHT_LED4, 0);
    digitalWrite(VOLTMETER_LEFT_LED5, 0);
    digitalWrite(VOLTMETER_RIGHT_LED5, 0);

    if(millis() % 1000 < 200) { // turn off left aim LED
        digitalWrite(VOLTMETER_LEFT_LED1, 1);
        digitalWrite(VOLTMETER_RIGHT_LED1, 1);
    } 
    else if(millis() % 1000 < 400) { // turn off left aim LED
        digitalWrite(VOLTMETER_LEFT_LED2, 1);
        digitalWrite(VOLTMETER_RIGHT_LED2, 1);
    }
    else if(millis() % 1000 < 600) { // turn off left aim LED
        digitalWrite(VOLTMETER_LEFT_LED3, 1);
        digitalWrite(VOLTMETER_RIGHT_LED3, 1);
    }
    else if(millis() % 1000 < 800) { // turn off left aim LED
        digitalWrite(VOLTMETER_LEFT_LED4, 1);
        digitalWrite(VOLTMETER_RIGHT_LED4, 1);
    }
    else { // turn off left aim LED
        digitalWrite(VOLTMETER_LEFT_LED5, 1);
        digitalWrite(VOLTMETER_RIGHT_LED5, 1);
    }
  }


  /* Jack tests */
  buttonState = digitalRead(INTRO_JACK_PIN);
  if (last_intro_jack_state != buttonState) {
    Serial.print("INTRO JACK SWITCH ");
    Serial.println(buttonState);
    last_intro_jack_state = buttonState;
  }
  buttonState = digitalRead(TUTORIAL_JACK_PIN);
  if (last_tutorial_jack_state != buttonState) {
    Serial.print("TUTORIAL JACK SWITCH ");
    Serial.println(buttonState);
    last_tutorial_jack_state = buttonState;
  }
  buttonState = digitalRead(GAME_JACK_PIN);
  if (last_game_jack_state != buttonState) {
    Serial.print("GAME JACK SWITCH ");
    Serial.println(buttonState);
    last_game_jack_state = buttonState;
  }
      
  /* LCD test */
  // Устанавливаем курсор на вторую строку и нулевой символ.
  lcd.setCursor(0, 1);
  // Выводим на экран количество секунд с момента запуска ардуины
  lcd.print("SCORE ");
  lcd.print(score);


  /* MP3 test */
  if (!teslaPlaying) {
    mp3_set_volume (30);
    delay (100);
    mp3_play ();
    teslaPlaying = true;
    
  }
  if (!stereoPlaying) {
    mp3_II_set_volume (15);
    delay (100);
    mp3_II_play ();
    stereoPlaying = true;
  }

  /* Servo Tests */
  if (pos > 0 && pos < 180 && direction > 0) {
    pos++;
  } else if (pos > 0 && pos < 180 && direction < 0) {
    pos--;
  } else if (pos == 0 && direction < 0) {
    direction *= -1;
    pos++;
  } else if (pos == 180 && direction > 0) {
    direction *= -1;
    pos--;
  }
  servo_right.write(pos);              // tell servo to go to position in variable 'pos'
  servo_left.write(abs(180 - pos));
  delay(15);

}
