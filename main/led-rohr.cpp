#include "Arduino.h"
#include <Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/gpio.h>

#include "menu.h"
#include "Button.h"
#include "RotaryEncoder.h"

#define PIXEL_PIN GPIO_NUM_14
#define PIXEL_NUM 10

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C

#define ROTARY_ENCODER_GPIO_A GPIO_NUM_16
#define ROTARY_ENCODER_GPIO_B GPIO_NUM_17
#define ROTARY_ENCODER_PULSES_PER_SNAP 4

#define BUTTON_GPIO GPIO_NUM_5

static const char *TAG = "led-rohr";

extern "C" void app_main()
{
  initArduino();

  Adafruit_NeoPixel pixels(PIXEL_NUM, PIXEL_PIN, NEO_GRBW + NEO_KHZ800);
  pixels.begin();
  pixels.clear();

  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();

  ScreenManager screenManager;
  screenManager.pushScreen(new MenuScreen());

  RotaryEncoder rotaryEncoder(ROTARY_ENCODER_GPIO_A, ROTARY_ENCODER_GPIO_B, ROTARY_ENCODER_PULSES_PER_SNAP);
  rotaryEncoder.onKnobTurn = [&screenManager](int count) {
    screenManager.onKnobTurn(count);
  };
  rotaryEncoder.onKnobTurnCorse = [&screenManager](int count) {
    screenManager.onKnobTurnCorse(count);
  };

  Button button(BUTTON_GPIO);
  button.onButtonPress = [&screenManager]() {
    screenManager.onKnobPress();
  };

  uint8_t r = 128;

  for (int i = 0; i < PIXEL_NUM; i++)
  {
    pixels.setPixelColor(i, pixels.Color(r, 0, 0, 0));
  }
  pixels.show();

  while (true)
  {
    rotaryEncoder.poll();
    button.poll();
    screenManager.draw(&display);

    delay(10);
  }
}
