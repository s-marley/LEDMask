// Cross hatching effect of random colors.

#include "Arduino.h"

class CrossHatch {
  public:
    CrossHatch(){};
    bool runPattern();
  private:
    struct Blobs {
      uint8_t _x;
      uint8_t _y;
      bool _right;
      CHSV _colour;
    };
    Blobs blob[20];
    uint8_t blobCounter = 0;
    const uint8_t blobRate = 5;     // Higher number is fewer blobs
    long previousTime = 0;
};

bool CrossHatch::runPattern() {
  checkBrightnessButton();
  if(checkModeButton()) return false;
  
  // Fade deals with 'tails'
  fadeToBlackBy(leds, NUM_LEDS, 5);

  if(millis() - previousTime >= 50) {
    //Spawn new horizontal blob
    if (random8(blobRate) == 0) {
      uint8_t spawnY = random8(kMatrixHeight);
      blob[blobCounter] = {0, spawnY, true, CHSV(random8(),255,255)};
      blobCounter = (blobCounter + 1) %20;
    }

    // Spawn new vertical blob
    if (random8(blobRate) == 0) {
      uint8_t spawnX = random8(kMatrixWidth);
      blob[blobCounter] = {spawnX, 0, false, CHSV(random8(),255,255)};
      blobCounter = (blobCounter + 1) %20;
    }

    // Draw the blobs
    for (int i = 0; i < 20; i++) {
      leds[XY(blob[i]._x, blob[i]._y)] = leds[XY(blob[i]._x, blob[i]._y)] + blob[i]._colour;
    }

    // Move the blobs
    for (int i = 0; i < 20; i++) {
      if(blob[i]._right) { blob[i]._x++; }
      else {blob[i]._y++; }
    }

    previousTime = millis();
  }

  FastLED.show();
  return true;
}
