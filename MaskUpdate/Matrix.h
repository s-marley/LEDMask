// Falling green 'code' like The Matrix.
// Adapted from https://gist.github.com/Jerware/b82ad4768f9935c8acfccc98c9211111

#include "Arduino.h"

class Matrix {
  public:
    Matrix(){};
    bool runPattern();
  private:
    long previousTime = 0;
};

bool Matrix::runPattern() {
  checkBrightnessButton();
  if(checkModeButton()) return false;
  if(millis() - previousTime >= 75) {
    // Move bright spots downward
    for (int row = kMatrixHeight - 1; row >= 0; row--) {
      for (int col = 0; col < kMatrixWidth; col++) {
        if (leds[XY(col, row)] == CRGB(175,255,175)) {
          leds[XY(col, row)] = CRGB(27,200,39); // create trail
          if (row < kMatrixHeight - 1) leds[XY(col, row + 1)] = CRGB(175,255,175);
        }
      }
    }
    
    // Fade all leds
    for(int i = 0; i < NUM_LEDS; i++) {
      if (leds[i].g != 255) leds[i].nscale8(192); // only fade trail
    }

    // Spawn new falling spots
    if (random8(2) == 0) // lower number == more frequent spawns
    {
      int8_t spawnX = random8(kMatrixWidth);
      leds[XY(spawnX, 0)] = CRGB(175,255,175 );
    }

    FastLED.show();
    previousTime = millis();
  }
  return true;
}
