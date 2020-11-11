
#include "Arduino.h"

class Snake {
  public:
    Snake();
    bool runPattern();
  private:
    long previousMillis = 0;
    uint8_t colorIndex;
    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    enum Dir {UP, DOWN, LEFT, RIGHT};
    struct Snakes{
      uint8_t x = 0;
      uint8_t y = 0;
      Dir d = UP;
    };
    const static uint8_t interval = 16;     // ms per frame [30]
    const static uint8_t numSnakes = 10;    // number of snakes to draw [10]
    const static uint8_t fadeRate = 10;     // How quickly to fade the trails (higher = faster fade) [20]
    const static uint8_t blendRate = 24;    // How quickly to blend between palettes (higher = slower blend) [24]
    const static uint8_t paletteTime = 5;   // Num seconds to show one palette [5]
    Snakes snakes[numSnakes];

    void fillSnakeWithColor();
    void nextStep(uint8_t i);
    void chooseNewDirection(uint8_t i);
    bool canPass(uint16_t ledIndex);
    void setupRandomPalette();
};

Snake::Snake(){
  for(int i = 0; i < numSnakes; i++) {
    uint8_t x;
    uint8_t y;
    do {
      x = random(kMatrixWidth);
      y = random(kMatrixHeight);
    } while(XY(x,y) > LAST_VISIBLE_LED);
    snakes[i].x = x;
    snakes[i].y = y;
    snakes[i].d = RIGHT;
  }
  setupRandomPalette();
}

bool Snake::runPattern() {
  checkBrightnessButton();
  if(checkModeButton()) return false;
  fadeToBlackBy(leds, NUM_LEDS, fadeRate);
  nblendPaletteTowardPalette( currentPalette, targetPalette, blendRate);
  fillSnakeWithColor();

  EVERY_N_SECONDS (paletteTime) {
    setupRandomPalette();
  }

  if(millis() - previousMillis >= interval) {    
    for(int i = 0; i < numSnakes; i++){
      nextStep(i);
    }
    previousMillis = millis();
  }

  FastLED.show();
  return true;
}

void Snake::fillSnakeWithColor() {
  for (int i = 0; i < numSnakes; i++) {
    leds[XY(snakes[i].x, snakes[i].y)] = ColorFromPalette( currentPalette, colorIndex );
    colorIndex++;
  }
  
}

void Snake::nextStep(uint8_t i){

  // Give small chance to change direction randomly
  if(random(5) == 0){
    chooseNewDirection(i);
  }

  // Check if the next move is valid if not, change direction
  bool failed = true;
  uint16_t nextLED = 0;
  while(failed){
    switch(snakes[i].d){
        case UP:
          nextLED = XY(snakes[i].x, snakes[i].y-1);
          if(nextLED <= LAST_VISIBLE_LED || canPass(nextLED)) {
            snakes[i].y = snakes[i].y-1;
            failed = false;
            break;
          }
          chooseNewDirection(i);
          break;
        case RIGHT:
          nextLED = XY(snakes[i].x+1, snakes[i].y);
          if(nextLED <= LAST_VISIBLE_LED || canPass(nextLED)) {
            snakes[i].x = snakes[i].x+1;
            failed = false;
            break;
          }
          chooseNewDirection(i);
          break;
        case DOWN:
          nextLED = XY(snakes[i].x, snakes[i].y+1);
          if(nextLED <= LAST_VISIBLE_LED || canPass(nextLED)) {
            snakes[i].y = snakes[i].y+1;
            failed = false;
            break;
          }
          chooseNewDirection(i);
          break;
        case LEFT:
          nextLED = XY(snakes[i].x-1, snakes[i].y);
          if(nextLED <= LAST_VISIBLE_LED || canPass(nextLED)) {
            snakes[i].x = snakes[i].x-1;
            failed = false;
            break;
          }
          chooseNewDirection(i);
          break;
    }
  }
}

void Snake::chooseNewDirection(uint8_t i) {
  randomSeed(analogRead(0));
  Dir r;
  do {
    r = Dir(rand() % 4);
  } while(r == snakes[i].d);

  snakes[i].d = r;
}

bool Snake::canPass(uint16_t ledIndex) {
  uint16_t ok[] = {124,125,130,131,132,133};
  for (int i = 0; i < 6; i++) if(ledIndex == ok[i]) return true;
  return false;
}

void Snake::setupRandomPalette() {
  // Set the initial random hue, then add some randomness to each part of the palette
  // This way the hues are 'close together' so they look better next to each other.
  uint8_t hue = random8();
  targetPalette = CRGBPalette16( 
                      CHSV( hue + random8(50), 255, 32),      // Dim
                      CHSV( hue + random8(50), 255, 255),     // Bright
                      CHSV( hue + random8(50), 128, 255),     // Bright pastel
                      CHSV( hue + random8(50), 255, 255));    // Bright
}
