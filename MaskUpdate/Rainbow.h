// Basic rainbow effect.

#include "Arduino.h"

class Rainbow {
  public:
    Rainbow(){};
    bool runPattern();
  private:
    void drawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
};

bool Rainbow::runPattern() {
  checkBrightnessButton();
  if(checkModeButton()) return false;
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t) cos16(ms * (27 / 1)) * (350 / kMatrixWidth));
  int32_t xHueDelta32 = ((int32_t) cos16(ms * (39 / 1)) * (310 / kMatrixHeight));
  drawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  FastLED.show();
  return true;
}

void Rainbow::drawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
  byte lineStartHue = startHue8;
  for (byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;
    for (byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
    }
  }
}
