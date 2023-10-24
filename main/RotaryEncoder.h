#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

#include <driver/pulse_cnt.h>

#define EXAMPLE_PCNT_LOW_LIMIT -20000
#define EXAMPLE_PCNT_HIGH_LIMIT 20000

class RotaryEncoder
{
protected:
  int pulses_per_snap;
  pcnt_unit_handle_t pcnt_unit;

  int last_count = 0;
  int last_corse_count = 0;

  static constexpr const char *TAG = "RotaryEncoder";

public:
  std::function<void(int)> onKnobTurn = nullptr;
  std::function<void(int)> onKnobTurnCorse = nullptr;

  RotaryEncoder(gpio_num_t gpio_a, gpio_num_t gpio_b, int pulses_per_snap)
  {
    this->pulses_per_snap = pulses_per_snap;

    pcnt_unit_config_t unit_config = {
      .low_limit = EXAMPLE_PCNT_LOW_LIMIT,
      .high_limit = EXAMPLE_PCNT_HIGH_LIMIT,
      .flags = { },
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    ESP_LOGI(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 2000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_LOGI(TAG, "install pcnt channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = gpio_b,
        .level_gpio_num = gpio_a,
        .flags = { },
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = gpio_a,
        .level_gpio_num = gpio_b,
        .flags = { },
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_LOGI(TAG, "enable pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_LOGI(TAG, "clear pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_LOGI(TAG, "start pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
  }

  void poll() {
    int count = 0;
    int corse_count = 0;

    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &count));
    corse_count = count / pulses_per_snap;

    if (last_corse_count != corse_count)
    {
      if (onKnobTurnCorse != nullptr)
      {
        onKnobTurnCorse(corse_count - last_corse_count);
      }
    }

    if (last_count != count)
    {
      if (onKnobTurn != nullptr)
      {
        onKnobTurn(count - last_count);
      }
    }

    last_count = count;
    last_corse_count = corse_count;
  }
};

#endif
