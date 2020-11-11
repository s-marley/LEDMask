// Like water drops using palettes.

#include "Arduino.h"

class Drops {
  public:
    Drops();
    bool runPattern();
  private:
    void drawCircle(int xc, int yc, int r, CRGB color);
    void changePalette();
    uint8_t paletteNumber = 0;
    
    struct Ripple {
      uint8_t _x = 0;
      uint8_t _y = 0 ;
      uint8_t _r = 0;
      uint8_t _index = 0;
    };
    Ripple ripple[20];
    uint8_t rippleCounter = 0;
    uint8_t rippleRate = 10;     // Higher number is fewer ripples
    long previousTime = 0;
    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    
};

Drops::Drops() {
  currentPalette = OceanColors_p;
}

bool Drops::runPattern() {
  checkBrightnessButton();
  if(checkModeButton()) return false;
  fadeToBlackBy( leds, NUM_LEDS, 20);
  nblendPaletteTowardPalette( currentPalette, targetPalette, 24);

  EVERY_N_SECONDS(5) {
    paletteNumber = (paletteNumber + 1) %5;
    changePalette();
  }
  
  if(millis() - previousTime >= 30) {
    
    // Spawn new ripple
    if (random8(rippleRate) == 0 ) {
      int8_t spawnX = random8(kMatrixWidth);
      int8_t spawnY = random8(kMatrixHeight);
      ripple[rippleCounter]._x = spawnX;
      ripple[rippleCounter]._y = spawnY;
      ripple[rippleCounter]._r = 1;
      ripple[rippleCounter]._index = 0;
      rippleCounter = (rippleCounter + 1) %20;
    }

    // Draw ripple and spread out
    for (int i = 0; i < 20;i++) {
      if(ripple[i]._r < MAX_DIMENSION) {
        CRGB color = ColorFromPalette( currentPalette, ripple[i]._index, 255);
        drawCircle(ripple[i]._x, ripple[i]._y, ripple[i]._r, color);
        ripple[i]._r++;
        ripple[i]._index += 20;
      }
    }

    previousTime = millis();
  }
  
  FastLED.show();
  return true;
}

void Drops::drawCircle(int xc, int yc, int r, CRGB color)
{
  int x = -r;
  int y = 0;
  int e = 2 - (2 * r);
  do
  {
    leds[XY(xc + x, yc - y)] = color;
    leds[XY(xc - x, yc + y)] = color;
    leds[XY(xc + y, yc + x)] = color;
    leds[XY(xc - y, yc - x)] = color;
    int _e = e;
    if (_e <= y)
      e += (++y * 2) + 1;
    if ((_e > x) || (e > y))
      e += (++x * 2) + 1;
  }
  while (x < 0);
}

void Drops::changePalette() {
  switch(paletteNumber) {
    case 0:
      targetPalette = OceanColors_p;
      break;
    case 1:
      targetPalette = PartyColors_p;
      break;
    case 2:
      targetPalette = ForestColors_p;
      break;
    case 3:
      targetPalette = HeatColors_p;
      break;
    case 4:
      targetPalette = RainbowColors_p;
      break;
  }
}
