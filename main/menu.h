#ifndef MAINMENUSCREEN_H
#define MAINMENUSCREEN_H

#include <array>

#include "ScreenManager.h"

void defaultTextSettings(Adafruit_SSD1306 *display)
{
  display->setCursor(0, 0);
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
}

void renderLabeledValue(Adafruit_SSD1306 *display, String label, String value)
{
    defaultTextSettings(display);

    display->setCursor(0, 2);
    display->println(label);

    display->setCursor(0, 14);
    display->setTextSize(2);
    display->println(value);
}

class DmxChannelMenuScreen : public Screen
{
protected:
  int value = 0;

public:
  void draw(Adafruit_SSD1306 *display)
  {
    display->clearDisplay();
    renderLabeledValue(display, "DMX Channel", String(value));
    display->display();
  }

  void onKnobTurnCorse(int count)
  {
    value = (value + count) % 512;
  }

  void onKnobPress()
  {
    screenManager->popScreen();
  }
};

class DmxUniverseMenuScreen : public Screen
{
protected:
  int value = 0;

public:
  void draw(Adafruit_SSD1306 *display)
  {
    display->clearDisplay();
    renderLabeledValue(display, "DMX Universe", String(value));
    display->display();
  }

  void onKnobTurnCorse(int count)
  {
    value = (value + count) % 512;
  }

  void onKnobPress()
  {
    screenManager->popScreen();
  }
};

struct MenuPage
{
  String title;
  std::function<void()> action;
};

class MenuScreen : public Screen
{
protected:
  int currentPageIdx = 0;
  std::array<MenuPage, 2> pages;
  DmxChannelMenuScreen dmxChannelMenuScreen;
  DmxUniverseMenuScreen dmxUniverseMenuScreen;

public:
  void init()
  {
    this->pages = std::array<MenuPage, 2>{
      MenuPage {
        .title = "DMX\nChannel",
        .action = [this]() {
          screenManager->pushScreen(&dmxChannelMenuScreen);
        }
      },
      MenuPage {
        .title = "DMX\nUniverse",
        .action = [this]() {
          screenManager->pushScreen(&dmxUniverseMenuScreen);
        }
      }
    };
  }

  void draw(Adafruit_SSD1306 *display)
  {
    display->clearDisplay();
    defaultTextSettings(display);
    display->setCursor(0, 1);
    display->setTextSize(2);
    display->println(pages[currentPageIdx].title);
    display->display();
  }

  void onKnobPress()
  {
    pages[currentPageIdx].action();
  }

  void onKnobTurnCorse(int count)
  {
    this->currentPageIdx = (this->currentPageIdx + count) % this->pages.size();
  }
};

#endif
