#include "Arduino.h"
#include <Adafruit_NeoPixel/Adafruit_NeoPixel.h>

const int PIN = 14;
#define NUMPIXELS 10


extern "C" void app_main() {
	initArduino();

	Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

	pixels.begin();
	pixels.clear();

  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0, 255));
  }
	pixels.show();
}
