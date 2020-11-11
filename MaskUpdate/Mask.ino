/* Helper functions for an two-dimensional XY matrix of pixels.

This special 'XY' code lets you program the RGB Shades
if it's was just a plain 16x5 matrix.  

Writing to and reading from the 'holes' in the layout is 
also allowed; holes retain their data, it's just not displayed.

You can also test to see if you're on- or off- the layout
like this
  if( XY(x,y) > LAST_VISIBLE_LED ) { ...off the layout...}

X and Y bounds checking is also included, so it is safe
to just do this without checking x or y in your code:
  leds[ XY(x,y) ] == CRGB::Red;
All out of bounds coordinates map to the first hidden pixel.

 https://macetech.github.io/FastLED-XY-Map-Generator/
      0   1   2   3   4   5   6   7   8   9  10  11   12 13  14
   +-----------------------------------------------------------
 0 | 14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
 1 | 15  16  17  18  19  20  21  22  23  24  25  26  27  28  29
 2 | 44  43  42  41  40  39  38  37  36  35  34  33  32  31  30
 3 | 45  46                      47                      48  49
 4 | 58  57  56              55  54  53              52  51  50
 5 | 59  60  61  62          63  64  65          66  67  68  69
 6 |     80  79  78      77  76  75  74  73      72  71  70  
 7 |                     81  82  83  84  85          
 8 |                 92  91  90  89  88  87  86        
 9 |                     93  94  95  96  97          
10 |                     102 101 100 99  98    
*/

#include <FastLED.h>
#include <EEPROM.h>
#include <JC_Button.h>

#define LED_PIN           5           // Output pin for LEDs [5]
#define MODE_PIN          3           // Input pin for button to change mode [3]
#define PLUS_PIN          2           // Input pin for plus button [2]
#define MINUS_PIN         4           // Input pin for minus button [4]
#define MIC_PIN           A6          // Input pin for microphone [A6]
#define COLOR_ORDER       GRB         // Color order of LED string [GRB]
#define CHIPSET           WS2812B     // LED string type [WS2182B]
#define BRIGHTNESS        50          // Overall brightness [50]
#define LAST_VISIBLE_LED  102         // Last LED that's visible [102]
#define MAX_MILLIAMPS     5000        // Max current in mA to draw from supply [500]
#define SAMPLE_WINDOW     100         // How many ms to sample audio for [100]
#define DEBOUNCE_MS       20          // Number of ms to debounce the button [20]
#define LONG_PRESS        500         // Number of ms to hold the button to count as long press [500]
#define PATTERN_TIME      10          // Seconds to show each pattern on autoChange [10]
#define kMatrixWidth      15          // Matrix width [15]
#define kMatrixHeight     11          // Matrix height [11]
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)                                       // Total number of Leds
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)   // Largest dimension of matrix

CRGB leds[ NUM_LEDS ];
uint8_t brightness = BRIGHTNESS;
uint8_t soundSensitivity = 10;

// Used to check RAM availability. Usage: Serial.println(freeRam());
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Button stuff
uint8_t buttonPushCounter = 10;
uint8_t state = 0;
bool autoChangeVisuals = false;
Button modeBtn(MODE_PIN, DEBOUNCE_MS);
Button plusBtn(PLUS_PIN, DEBOUNCE_MS);
Button minusBtn(MINUS_PIN, DEBOUNCE_MS);

void incrementButtonPushCounter() {
  buttonPushCounter = (buttonPushCounter + 1) %11;
  EEPROM.write(1, buttonPushCounter);
}

// Include various patterns
#include "Sound.h"
#include "Rainbow.h"
#include "Fire.h"
#include "Squares.h"
#include "Circles.h"
#include "Plasma.h"
#include "Matrix.h"
#include "CrossHatch.h"
#include "Drops.h"
#include "Noise.h"
#include "Snake.h"

// Helper to map XY coordinates to irregular matrix
uint16_t XY( uint8_t x, uint8_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t XYTable[] = {
    14,  13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
    44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32,  31,  30,
    45,  46, 103, 104, 105, 106, 107,  47, 108, 109, 110, 111, 112,  48,  49,
    58,  57,  56, 118, 117, 116,  55,  54,  53, 115, 114, 113,  52,  51,  50,
    59,  60,  61,  62, 119, 120,  63,  64,  65, 121, 122,  66,  67,  68,  69,
   126,  80,  79,  78, 125,  77,  76,  75,  74,  73, 124,  72,  71,  70, 123,
   127, 128, 129, 130, 131,  81,  82,  83,  84,  85, 132, 133, 134, 135, 136,
   144, 143, 142, 141,  92,  91,  90,  89,  88,  87,  86, 140, 139, 138, 137,
   145, 146, 147, 148, 149,  93,  94,  95,  96,  97, 150, 151, 152, 153, 154,
   164, 163, 162, 161, 160, 102, 101, 100,  99,  98, 159, 158, 157, 156, 155
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XYTable[i];
  return j;
}

void setup() {
  FastLED.addLeds < CHIPSET, LED_PIN, COLOR_ORDER > (leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(brightness);
  FastLED.clear(true);

  modeBtn.begin();
  plusBtn.begin();
  minusBtn.begin();
  buttonPushCounter = (int)EEPROM.read(1);    // load previous setting
  
  Serial.begin(57600);
  Serial.print(F("Starting pattern "));
  Serial.println(buttonPushCounter);
}

void checkBrightnessButton() {
  // On all none-sound reactive patterns, plus and minus control the brightness
  plusBtn.read();
  minusBtn.read();

  if (plusBtn.wasReleased()) {
    brightness += 20;
    FastLED.setBrightness(brightness);
  }

  if (minusBtn.wasReleased()) {
    brightness -= 20;
    FastLED.setBrightness(brightness);
  }
}

void checkSoundLevelButton(){
  // On all sound reactive patterns, plus and minus control the sound sensitivity
  plusBtn.read();
  minusBtn.read();

  if (plusBtn.wasReleased()) {
    // Increase sound sensitivity
    soundSensitivity -= 1;
    if (soundSensitivity == 0) soundSensitivity = 1;
  }

  if (minusBtn.wasReleased()) {
    // Decrease sound sensitivity
    soundSensitivity += 1;
  }
}

bool checkModeButton() {  
  modeBtn.read();

  switch (state) {
    case 0:                
      if (modeBtn.wasReleased()) {
        incrementButtonPushCounter();
        Serial.print(F("Short press, pattern "));
        Serial.println(buttonPushCounter);
        autoChangeVisuals = false;
        return true;
      }
      else if (modeBtn.pressedFor(LONG_PRESS)) {
        state = 1;
        return true;
      }
      break;

    case 1:
      if (modeBtn.wasReleased()) {
        state = 0;
        Serial.print(F("Long press, auto, pattern "));
        Serial.println(buttonPushCounter);
        autoChangeVisuals = true;
        return true;
      }
      break;
  }

  if(autoChangeVisuals){
    EVERY_N_SECONDS(PATTERN_TIME) {
      incrementButtonPushCounter();
      Serial.print("Auto, pattern ");
      Serial.println(buttonPushCounter); 
      return true;
    }
  }  

  return false;
}

// Functions to run patterns. Done this way so each class stays in scope only while
// it is active, freeing up RAM once it is changed.

void runSound(){
  bool isRunning = true;
  Sound sound = Sound();
  while(isRunning) isRunning = sound.runPattern();
}

void runRainbow(){
  bool isRunning = true;
  Rainbow rainbow = Rainbow();
  while(isRunning) isRunning = rainbow.runPattern();
}

void runFire(){
  bool isRunning = true;
  Fire fire = Fire();
  while(isRunning) isRunning = fire.runPattern();
}

void runSquares(){
  bool isRunning = true;
  Squares squares = Squares();
  while(isRunning) isRunning = squares.runPattern();
}

void runCircles(){
  bool isRunning = true;
  Circles circles = Circles();
  while(isRunning) isRunning = circles.runPattern();
}

void runPlasma(){
  bool isRunning = true;
  Plasma plasma = Plasma();
  while(isRunning) isRunning = plasma.runPattern();
}

void runMatrix(){
  bool isRunning = true;
  Matrix matrix = Matrix();
  while(isRunning) isRunning = matrix.runPattern();
}

void runCrossHatch(){
  bool isRunning = true;
  CrossHatch crossHatch = CrossHatch();
  while(isRunning) isRunning = crossHatch.runPattern();
}

void runDrops(){
  bool isRunning = true;
  Drops drops = Drops();
  while(isRunning) isRunning = drops.runPattern();
}

void runNoise(){
  bool isRunning = true;
  Noise noise = Noise();
  while(isRunning) {
    isRunning = noise.runPattern();
  }
}

void runSnake(){
  bool isRunning = true;
  Snake snake = Snake();
  while(isRunning) {
    isRunning = snake.runPattern();
  }
}

// Run selected pattern
void loop() {
  switch (buttonPushCounter) {
    case 0:
      runSound();
      break;
    case 1:
      runRainbow();
      break;
    case 2:
      runFire();
      break;
    case 3:
      runSquares();
      break;
    case 4:
      runCircles();
      break;
    case 5:
      runPlasma();
      break;
    case 6:
      runMatrix();
      break;
    case 7:
      runCrossHatch();
      break;
    case 8:
      runDrops();
      break;
    case 9:
      runNoise();
      break;
    case 10:
      runSnake();
      break;
  }
}
