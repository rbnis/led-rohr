#ifndef SETTINGS_H
#define SETTINGS_H

#include <Preferences.h>

class Settings
{
public:
  unsigned int dmxChannel = 1;
  unsigned int dmxUniverse = 1;

  Settings()
  {
    load();
  }

  void load()
  {
    Preferences preferences;
    preferences.begin("settings", false);
    dmxChannel = preferences.getUInt("dmx_channel", 1);
    dmxUniverse = preferences.getUInt("dmx_universe", 1);
    preferences.end();
  }

  void save()
  {
    Preferences preferences;
    preferences.begin("settings", false);
    preferences.putUInt("dmx_channel", dmxChannel);
    preferences.putUInt("dmx_universe", dmxUniverse);
    preferences.end();
  }
};

#endif
