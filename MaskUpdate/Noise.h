// INtereesting use of the FastLED noise function.
// Adapted from https://github.com/FastLED/FastLED/blob/master/examples/NoisePlusPalette/NoisePlusPalette.ino

#include "Arduino.h"

class Noise {
  public:
    Noise();
    bool runPattern();
  private:
    uint16_t x = random16();  // 16 bit version of coordinates
    uint16_t y = random16();
    uint16_t z = random16();
    uint16_t speed = 20; // speed is set dynamically once we've started up
    uint16_t scale = 30; // scale is set dynamically once we've started up
    uint8_t noise[MAX_DIMENSION][MAX_DIMENSION]; // This is the array that we keep our computed noise values in
    CRGBPalette16 currentPalette; // Starting palette
    CRGBPalette16 targetPalette;  // Target palette to blend to
    uint8_t colorLoop = 1;
    
    void fillnoise8();
    void mapNoiseToLEDsUsingPalette();
    void ChangePaletteAndSettingsPeriodically();
    void SetupRandomPalette();
    void SetupPurpleAndGreenPalette();
    void SetupBlackAndWhiteStripedPalette();
   
};

Noise::Noise (){
  currentPalette = PartyColors_p;
}

bool Noise::runPattern() {
  checkBrightnessButton();
  if(checkModeButton()) return false;
  nblendPaletteTowardPalette( currentPalette, targetPalette, 24);
  ChangePaletteAndSettingsPeriodically();
  fillnoise8();
  mapNoiseToLEDsUsingPalette();

  LEDS.show();
  return true;
}

void Noise::fillnoise8() {
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  if( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }
  
  for(int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;
      
      uint8_t data = inoise8(x + ioffset,y + joffset,z);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data,16);
      data = qadd8(data,scale8(data,39));

      if( dataSmoothing ) {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }
      
      noise[i][j] = data;
    }
  }
  
  z += speed;
  
  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;
}

void Noise::mapNoiseToLEDsUsingPalette()
{
  static uint8_t ihue=0;
  
  for(int i = 0; i < kMatrixWidth; i++) {
    for(int j = 0; j < kMatrixHeight; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri =   noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) { 
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
      leds[XY(i,j)] = color;
    }
  }
  
  ihue+=1;
}

void Noise::ChangePaletteAndSettingsPeriodically()
{
  uint8_t secondHand = ((millis() / 1000) / 1) % 60;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    if( secondHand ==  0)  { targetPalette = RainbowColors_p;         speed = 20; scale = 30; colorLoop = 1; }
    if( secondHand ==  5)  { SetupPurpleAndGreenPalette();             speed = 10; scale = 50; colorLoop = 1; }
    if( secondHand == 10)  { SetupBlackAndWhiteStripedPalette();       speed = 20; scale = 30; colorLoop = 1; }
    if( secondHand == 15)  { targetPalette = ForestColors_p;          speed =  8; scale =30; colorLoop = 0; }
    if( secondHand == 20)  { targetPalette = CloudColors_p;           speed =  4; scale = 30; colorLoop = 0; }
    if( secondHand == 25)  { targetPalette = LavaColors_p;            speed =  8; scale = 50; colorLoop = 0; }
    if( secondHand == 30)  { targetPalette = OceanColors_p;           speed = 20; scale = 90; colorLoop = 0; }
    if( secondHand == 35)  { targetPalette = PartyColors_p;           speed = 20; scale = 30; colorLoop = 1; }
    if( secondHand == 40)  { SetupRandomPalette();                     speed = 20; scale = 20; colorLoop = 1; }
    if( secondHand == 45)  { SetupRandomPalette();                     speed = 50; scale = 50; colorLoop = 1; }
    if( secondHand == 50)  { SetupRandomPalette();                     speed = 20; scale = 50; colorLoop = 1; }
    if( secondHand == 55)  { targetPalette = RainbowStripeColors_p;   speed = 30; scale = 20; colorLoop = 1; }
  }
}

// This function generates a random palette that's a gradient
// between four different colors.  The first is a dim hue, the second is 
// a bright hue, the third is a bright pastel, and the last is 
// another bright hue.  This gives some visual bright/dark variation
// which is more interesting than just a gradient of different hues.
void Noise::SetupRandomPalette()
{
  targetPalette = CRGBPalette16( 
                      CHSV( random8(), 255, 32), 
                      CHSV( random8(), 255, 255), 
                      CHSV( random8(), 128, 255), 
                      CHSV( random8(), 255, 255)); 
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void Noise::SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( targetPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  targetPalette[0] = CRGB::White;
  targetPalette[4] = CRGB::White;
  targetPalette[8] = CRGB::White;
  targetPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void Noise::SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  
  targetPalette = CRGBPalette16( 
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black );
}
