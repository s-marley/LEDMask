#include <FastLED.h>

#define LED_PIN  5
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS 64

const uint8_t kMatrixWidth = 15;
const uint8_t kMatrixHeight = 11;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds[NUM_LEDS];

#define LAST_VISIBLE_LED 102

uint8_t XY (uint8_t x, uint8_t y) {
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
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
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
}

void loop()
{
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
  int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
  DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  if( ms < 5000 ) {
    FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
  } else {
    FastLED.setBrightness(BRIGHTNESS);
  }
  FastLED.show();
}

void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue, 255, 255);
    }
  }
}
