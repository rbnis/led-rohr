#include "Arduino.h"
#include <Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/pulse_cnt.h>
#include <driver/gpio.h>

const int PIN = 14;
#define NUMPIXELS 10

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PULSES_PER_SNAP 4

#define EXAMPLE_PCNT_LOW_LIMIT -20000
#define EXAMPLE_PCNT_HIGH_LIMIT 20000

#define ROTARY_ENCODER_GPIO_A GPIO_NUM_16
#define ROTARY_ENCODER_GPIO_B GPIO_NUM_17

#define BUTTON_GPIO GPIO_NUM_5

static const char *TAG = "led-rohr";

enum MenuItem
{
  dmx_channel,
  dmx_universe
};

void initKnob(pcnt_unit_handle_t *pcnt_unit)
{
  pcnt_unit_config_t unit_config = {
      .low_limit = EXAMPLE_PCNT_LOW_LIMIT,
      .high_limit = EXAMPLE_PCNT_HIGH_LIMIT,
  };
  ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, pcnt_unit));

  ESP_LOGI(TAG, "set glitch filter");
  pcnt_glitch_filter_config_t filter_config = {
      .max_glitch_ns = 2000,
  };
  ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(*pcnt_unit, &filter_config));

  ESP_LOGI(TAG, "install pcnt channels");
  pcnt_chan_config_t chan_a_config = {
      .edge_gpio_num = ROTARY_ENCODER_GPIO_B,
      .level_gpio_num = ROTARY_ENCODER_GPIO_A,
  };
  pcnt_channel_handle_t pcnt_chan_a = NULL;
  ESP_ERROR_CHECK(pcnt_new_channel(*pcnt_unit, &chan_a_config, &pcnt_chan_a));
  pcnt_chan_config_t chan_b_config = {
      .edge_gpio_num = ROTARY_ENCODER_GPIO_A,
      .level_gpio_num = ROTARY_ENCODER_GPIO_B,
  };
  pcnt_channel_handle_t pcnt_chan_b = NULL;
  ESP_ERROR_CHECK(pcnt_new_channel(*pcnt_unit, &chan_b_config, &pcnt_chan_b));

  ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
  ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
  ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
  ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
  ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

  ESP_LOGI(TAG, "enable pcnt unit");
  ESP_ERROR_CHECK(pcnt_unit_enable(*pcnt_unit));
  ESP_LOGI(TAG, "clear pcnt unit");
  ESP_ERROR_CHECK(pcnt_unit_clear_count(*pcnt_unit));
  ESP_LOGI(TAG, "start pcnt unit");
  ESP_ERROR_CHECK(pcnt_unit_start(*pcnt_unit));
}

MenuItem navigate_menu(MenuItem current_item, int count)
{
  if (count > 0)
  {
    return (MenuItem)((current_item + 1) % 2);
  }
  else if (count < 0)
  {
    return (MenuItem)((current_item + 1) % 2);
  }
  else
  {
    return current_item;
  }
}

String menu_item_to_string(MenuItem item)
{
  switch (item)
  {
  case dmx_channel:
    return "DMX Channel";
  case dmx_universe:
    return "DMX Universe";
  default:
    return "unknown";
  }
}

extern "C" void app_main()
{
  initArduino();

  Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

  pixels.begin();
  pixels.clear();

  uint8_t r = 128;

  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(r, 0, 0, 0));
  }
  pixels.show();

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("Hello world!");
  display.display();

  pcnt_unit_handle_t pcnt_unit = NULL;
  initKnob(&pcnt_unit);

  int count = 0;
  int last_count = 0;

  int corse_count = 0;
  int last_corse_count = 0;

  int setting_dmx_channel = 0;
  int setting_dmx_universe = 0;

  bool last_button_state = false;
  unsigned int last_button_change = 0;

  bool change_value = false;

  MenuItem current_menu_item = dmx_channel;

  gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
  gpio_set_level(BUTTON_GPIO, 1);

  while (true)
  {
    last_count = count;
    last_corse_count = corse_count;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &count));
    corse_count = count / PULSES_PER_SNAP;

    bool button_state = !gpio_get_level(BUTTON_GPIO);
    if (button_state && last_button_state == false) {
      unsigned int current_time = millis();

      if (current_time - last_button_change > 100) {
        last_button_change = current_time;

        ESP_LOGI(TAG, "button pressed");
        change_value = !change_value;
      }
    }
    last_button_state = button_state;

    if (change_value)
    {
      int value = 0;

      if (current_menu_item == dmx_channel)
      {
        setting_dmx_channel += corse_count - last_corse_count;
        value = setting_dmx_channel;
      }
      else if (current_menu_item == dmx_universe)
      {
        setting_dmx_universe += corse_count - last_corse_count;
        value = setting_dmx_universe;
      }

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("value: " + String(value));
      display.display();
    }
    else
    {
      current_menu_item = navigate_menu(current_menu_item, corse_count - last_corse_count);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(menu_item_to_string(current_menu_item));
      display.display();
    }

    delay(10);
  }
}
