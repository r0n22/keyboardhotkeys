#include <Keyboard.h>
/******************************************************************************
red-plus-buttons.ino
Byron Jacquot @ SparkFun Electronics
1/6/2015

Example to drive the red LEDs and scan the buttons of the RGB button pad.

Exercise 2 in a series of 3.
https://learn.sparkfun.com/tutorials/button-pad-hookup-guide/exercise-2-monochrome-plus-buttons

Development environment specifics:
Developed in Arduino 1.6.5
For an Arduino Mega 2560

This code is released under the [MIT License](http://opensource.org/licenses/MIT).

Distributed as-is; no warranty is given.
******************************************************************************/
//config variables
#define NUM_LED_COLUMNS (2)
#define NUM_LED_ROWS (1)
#define NUM_BTN_COLUMNS (2)
#define NUM_BTN_ROWS (1)
#define NUM_COLORS (1)
#define MAX_KEYS (2)

#define MAX_DEBOUNCE (3)

static char keys[NUM_BTN_ROWS][NUM_BTN_COLUMNS][MAX_KEYS] = {0};


// Global variables
static bool LED_buffer[NUM_LED_COLUMNS][NUM_LED_ROWS];

static const uint8_t btncolumnpins[NUM_BTN_COLUMNS] = { 7, 8 };
static const uint8_t btnrowpins[NUM_BTN_ROWS] = { 9 };
static const uint8_t ledcolumnpins[NUM_LED_COLUMNS] = { 14, 15 };
static const uint8_t colorpins[NUM_LED_ROWS] = { 10 };

static int8_t debounce_count[NUM_BTN_COLUMNS][NUM_BTN_ROWS];

static void setuppins() {
  uint8_t i;

  // initialize
  // select lines
  // LED columns
  for (i = 0; i < NUM_LED_COLUMNS; i++) {
    pinMode(ledcolumnpins[i], OUTPUT);

    // with nothing selected by default
    digitalWrite(ledcolumnpins[i], HIGH);
  }

  // button columns
  for (i = 0; i < NUM_BTN_COLUMNS; i++) {
    pinMode(btncolumnpins[i], OUTPUT);

    // with nothing selected by default
    digitalWrite(btncolumnpins[i], HIGH);
  }

  // button row input lines
  for (i = 0; i < NUM_BTN_ROWS; i++) {
    pinMode(btnrowpins[i], INPUT_PULLUP);
  }

  // LED drive lines
  for (i = 0; i < NUM_LED_ROWS; i++) {
    pinMode(colorpins[i], OUTPUT);
    digitalWrite(colorpins[i], LOW);
  }

  // Initialize the debounce counter array
  for (uint8_t i = 0; i < NUM_BTN_COLUMNS; i++) {
    for (uint8_t j = 0; j < NUM_BTN_ROWS; j++) {
      debounce_count[i][j] = 0;
    }
  }
}

static void scan() {
  static uint8_t current = 0;
  uint8_t val;
  uint8_t i, j,k;

  // Select current columns
  digitalWrite(btncolumnpins[current], LOW);
  digitalWrite(ledcolumnpins[current], LOW);

  // output LED row values
  for (i = 0; i < NUM_LED_ROWS; i++) {
    if (LED_buffer[current][i]) {
      digitalWrite(colorpins[i], HIGH);
    }
  }

  // pause a moment
  delay(1);

  // Read the button inputs
  for (j = 0; j < NUM_BTN_ROWS; j++) {
    val = digitalRead(btnrowpins[j]);

    if (val == LOW) {
      // active low: val is low when btn is pressed
      if (debounce_count[current][j] < MAX_DEBOUNCE) {
        debounce_count[current][j]++;
        if (debounce_count[current][j] == MAX_DEBOUNCE) {
          Serial.print("Key Down ");
          Serial.println((current * NUM_BTN_ROWS) + j);

          // Do whatever you want to with the button press here:
          // toggle the current LED state
          LED_buffer[current][j] = !LED_buffer[current][j];
        }
      }
    } else {
      // otherwise, button is released
      if (debounce_count[current][j] > 0) {
        debounce_count[current][j]--;
        if (debounce_count[current][j] == 0) {
          Serial.print("Key Up ");
          Serial.println((current * NUM_BTN_ROWS) + j);
          for(k=0 ; k < MAX_KEYS ; k++){
            char value = keys[j][current][k];
            if(value > 0){

              Serial.println(value);
              Keyboard.press(value);
            }
          }


          Keyboard.releaseAll();
          // If you want to do something when a key is released, do it here:

          LED_buffer[current][j] = !LED_buffer[current][j];
        }
      }
    }
  }  // for j = 0 to 3;

  delay(1);

  digitalWrite(btncolumnpins[current], HIGH);
  digitalWrite(ledcolumnpins[current], HIGH);

  for (i = 0; i < NUM_LED_ROWS; i++) {
    digitalWrite(colorpins[i], LOW);
  }

  current++;
  if (current >= NUM_LED_COLUMNS) {
    current = 0;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  keys[0][0][0] = 'A';
  keys[0][0][1] = 'B';
  keys[0][1][0] = 'D';
  Keyboard.begin();

  Serial.print("Starting Setup...");

  // setup hardware
  setuppins();

  // init global variables
  for (uint8_t i = 0; i < NUM_LED_COLUMNS; i++) {
    for (uint8_t j = 0; j < NUM_LED_ROWS; j++) {
      LED_buffer[i][j] = 1;
    }
  }

  Serial.println("Setup Complete.");
}

void loop() {
  // put your main code here, to run repeatedly:

  scan();
}
