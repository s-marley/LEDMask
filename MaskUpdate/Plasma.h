// Rainbow colored plasma.
// Adapted from https://github.com/AaronLiddiment/RGBLEDS/blob/master/LEDSprites/examples/Tetris/Tetris.ino

#include "Arduino.h"

class Plasma {
  public:
    Plasma(){};
    bool runPattern();
  private:
    uint16_t _plasmaShift = (random8(0, 5) * 32) + 64;
    uint16_t _plasmaTime = 0;
    const uint8_t _plasmaXfactor = 8;
    const uint8_t _plasmaYfactor = 8;
};

bool Plasma::runPattern() {
  checkBrightnessButton();
  if(checkModeButton()) return false;
  // Fill background with dim plasma
  for (int16_t x = 0; x < kMatrixWidth; x++) {
    for (int16_t y = 0; y < kMatrixHeight; y++) {
      int16_t r = sin16(_plasmaTime) / 256;
      int16_t h = sin16(x * r * _plasmaXfactor + _plasmaTime) + cos16(y * (-r) * _plasmaYfactor + _plasmaTime) + sin16(y * x * (cos16(-_plasmaTime) / 256) / 2);
      leds[XY(x, y)] = CHSV((uint8_t)((h / 256) + 128), 255, 255);
    }
  }
  uint16_t oldPlasmaTime = _plasmaTime;
  _plasmaTime += _plasmaShift;
  if (oldPlasmaTime > _plasmaTime)
  _plasmaShift = (random8(0, 5) * 32) + 64;

  FastLED.show();
  return true;
}
