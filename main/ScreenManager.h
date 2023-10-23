#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <vector>

#include <esp_log.h>
#include <Adafruit_SSD1306.h>

class ScreenManager;

class Screen
{
public:
  ScreenManager *screenManager = nullptr;
  bool initialized = false;

  virtual void init(){};
  virtual void onKnobPress(){};
  virtual void onKnobTurn(int count){};
  virtual void onKnobTurnCorse(int count){};
  virtual void draw(Adafruit_SSD1306 *display) = 0;
};

class ScreenManager
{
protected:
  Screen *currentScreen = nullptr;
  std::vector<Screen *> screenStack;

public:
  void onKnobPress()
  {
    if (currentScreen != nullptr)
    {
      currentScreen->onKnobPress();
    }
  }

  void onKnobTurn(int count)
  {
    if (currentScreen != nullptr)
    {
      currentScreen->onKnobTurn(count);
    }
  }

  void onKnobTurnCorse(int count)
  {
    if (currentScreen != nullptr)
    {
      currentScreen->onKnobTurnCorse(count);
    }
  }

  void draw(Adafruit_SSD1306 *display)
  {
    if (currentScreen != nullptr)
    {
      currentScreen->draw(display);
    }
  }

  void pushScreen(Screen *screen)
  {
    if (!screen->initialized) {
      screen->screenManager = this;
      screen->init();
      screen->initialized = true;
    }

    currentScreen = screen;
    screenStack.push_back(screen);
  }

  void popScreen()
  {
    if (screenStack.size() > 0)
    {
      screenStack.pop_back();
      currentScreen = screenStack.back();
    }
  }
};

#endif
