#ifndef BUTTON_H
#define BUTTON_H

#include <driver/gpio.h>

class Button
{
protected:
  bool last_button_state = false;
  unsigned int last_button_change = 0;
  gpio_num_t gpio_num;

public:
  std::function<void()> onButtonPress = nullptr;

  Button(gpio_num_t gpio_num)
  {
    this->gpio_num = gpio_num;
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
    gpio_set_level(gpio_num, 1);
  }

  void poll()
  {
    bool button_state = !gpio_get_level(gpio_num);
    if (button_state && last_button_state == false)
    {
      unsigned int current_time = millis();

      if (current_time - last_button_change > 100)
      {
        last_button_change = current_time;

        if (onButtonPress != nullptr)
        {
          onButtonPress();
        }
      }
    }
    last_button_state = button_state;
  }
};

#endif
