// Sound responsive face. Uses microphone breakout with MAX4466 attached to pin A6.

#include "Arduino.h"

class Sound {
  public:
    Sound(){};
    bool runPattern();
  private:
    void readVolume();
    void drawEyebrows(uint8_t type);
    void drawFlatEyebrow(uint8_t x, uint8_t y);
    void drawSlantUpEyebrow(uint8_t x, uint8_t y);
    void drawSlantDownEyebrow(uint8_t x, uint8_t y);
    void drawMouth(uint8_t type);
    uint8_t faceType = 0;
    int sample = 0;
};

bool Sound::runPattern() {
  checkSoundLevelButton();
  if(checkModeButton()) return false;
  FastLED.clear();
  readVolume();
  drawEyebrows(faceType);
  drawMouth(faceType);
  FastLED.show();
  return true;
}

void Sound::readVolume(){
  unsigned long startMillis = millis();  // Start of sample window
  int peakToPeak = 0;   // peak-to-peak level

  int signalMax = 0;
  int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < SAMPLE_WINDOW) {
    sample = analogRead(MIC_PIN);
    if (sample < 1024) {  // toss out spurious readings
      if (sample > signalMax) {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin - 512;  // max - min = peak-peak amplitude
  peakToPeak = (peakToPeak < 0) ? 0 : peakToPeak;
  if(peakToPeak < 3 * soundSensitivity) {faceType = 0;}
  else if (peakToPeak < 25 * soundSensitivity) {faceType = 1;}
  else faceType = 2;
}

void Sound::drawEyebrows(uint8_t type) {

  switch (type) {
  case 0:
    drawFlatEyebrow(2, 2);
    drawFlatEyebrow(9, 2);
    break;

  case 1:
    drawSlantUpEyebrow(2, 2);
    drawSlantDownEyebrow(9, 2);
    break;

  case 2:
    drawSlantUpEyebrow(2, 1);
    drawSlantDownEyebrow(9, 1);
    break;
  }
}

void Sound::drawFlatEyebrow(uint8_t x, uint8_t y) {
  for (int i = x; i < x + 4; i++) {
    leds[XY(i, y)] = CRGB(255, 0, 0);
  }
}

void Sound::drawSlantUpEyebrow(uint8_t x, uint8_t y) {
  leds[XY(x, y)] = CRGB(255, 0, 0);
  leds[XY(x + 1, y)] = CRGB(255, 0, 0);
  leds[XY(x + 2, y - 1)] = CRGB(255, 0, 0);
  leds[XY(x + 3, y - 1)] = CRGB(255, 0, 0);
}

void Sound::drawSlantDownEyebrow(uint8_t x, uint8_t y) {
  leds[XY(x, y - 1)] = CRGB(255, 0, 0);
  leds[XY(x + 1, y - 1)] = CRGB(255, 0, 0);
  leds[XY(x + 2, y)] = CRGB(255, 0, 0);
  leds[XY(x + 3, y)] = CRGB(255, 0, 0);
}

void Sound::drawMouth(uint8_t type){
  const uint8_t topSmile[] = {79, 80, 60, 71, 70, 68, 81, 82, 83, 84, 85, 86, 92};
  const uint8_t quietSmile[] = {87, 88, 89, 90, 91};
  const uint8_t mediumSmile[] = {93, 94, 95, 96, 97};
  const uint8_t loudSmile[] = {98, 99, 100, 101, 102, 93, 97};
  
  // Draw top of lips regardless
  for (int i = 0; i < 13; i++) {
    leds[topSmile[i]] = CRGB(255, 0, 0);
  }

  switch (type) {
    // Small mouth
  case 0:
    for (int i = 0; i < 5; i++) {
      leds[quietSmile[i]] = CRGB(255, 0, 0);
    }
    break;

    // Medium mouth
  case 1:
    for (int i = 0; i < 5; i++) {
      leds[mediumSmile[i]] = CRGB(255, 0, 0);
    }
    break;

    // Large mouth
  case 2:
    for (int i = 0; i < 7; i++) {
      leds[loudSmile[i]] = CRGB(255, 0, 0);
    }
    break;
  }
}
