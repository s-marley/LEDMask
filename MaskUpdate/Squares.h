// Fast pulsing white squares from center.

#include "Arduino.h"

class Squares {
  public:
    Squares(){};
    bool runPattern();
  private:
    void drawLine(int x, int y, int x2, int y2, CRGB color);
    void drawRectangle(int x, int y, int x2, int y2, CRGB color);
    void drawPattern(uint8_t pattern);
    uint8_t _pattern = 0;
};

bool Squares::runPattern() {
  checkBrightnessButton();
  if(checkModeButton()) return false;
  EVERY_N_MILLISECONDS(100) {
    FastLED.clear();
    _pattern = (_pattern + 1) %2;
    drawPattern(_pattern);
    FastLED.show();
  }
  return true;
}

void Squares::drawPattern(uint8_t pattern) {
  switch (pattern){
    case 0:
      for (int i = 0; i < 6; i +=2) {
        drawRectangle(i, i, kMatrixWidth -1 - i, kMatrixHeight - 1 - i, CRGB(255,255,255));
      }
      break;

    case 1:
      for (int i = 1; i < 6; i +=2) {
        drawRectangle(i, i, kMatrixWidth -1 - i, kMatrixHeight - 1 - i, CRGB(255,255,255));
      }
      break; 
  }
}

void Squares::drawLine(int x,int y,int x2, int y2, CRGB color) {
  // Bresenhams algorithm http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
  int w = x2 - x ;
  int h = y2 - y ;
  int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0 ;
  if (w<0) dx1 = -1 ; else if (w>0) dx1 = 1 ;
  if (h<0) dy1 = -1 ; else if (h>0) dy1 = 1 ;
  if (w<0) dx2 = -1 ; else if (w>0) dx2 = 1 ;
  int longest = abs(w) ;
  int shortest = abs(h) ;
  if (!(longest>shortest)) {
    longest = abs(h) ;
    shortest = abs(w) ;
    if (h<0) dy2 = -1 ; else if (h>0) dy2 = 1 ;
    dx2 = 0 ;            
  }
  int numerator = longest >> 1 ;
  for (int i=0;i<=longest;i++) {
    leds[XY(x,y)] = color;
    numerator += shortest ;
    if (!(numerator<longest)) {
      numerator -= longest ;
      x += dx1 ;
      y += dy1 ;
    } else {
      x += dx2 ;
      y += dy2 ;
    }
  }
}

void Squares::drawRectangle(int x, int y, int x2, int y2, CRGB color)
{
  drawLine(x, y, x, y2, color);
  drawLine(x, y2, x2, y2, color);
  drawLine(x2, y2, x2, y, color);
  drawLine(x2, y, x, y, color);
}
