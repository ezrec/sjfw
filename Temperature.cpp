#include <Arduino.h>

#include "Temperature.h"

void Temperature::doreport()
{
  if(report_m == 0)
    return;
  unsigned long now = millis();
  if(report_l + report_m < now)
  {
    report_l = now;
    (*report_h).labelnum("T:",getHotend(),false);
    (*report_h).labelnum(" B:",getPlatform());
  }
}

Temperature::Temperature()
  : hotend_therm(HOTEND_TEMP_PIN, 0),
    platform_therm(PLATFORM_TEMP_PIN, 1),
    hotend_heat(HOTEND_HEAT_PIN),
    platform_heat(PLATFORM_HEAT_PIN)
{
  report_m = 0;
  report_l = 0;
  report_h = 0;
  hotend_therm.init();
  platform_therm.init();
  pinMode(hotend_heat, OUTPUT); digitalWrite(hotend_heat, false);
  pinMode(platform_heat, OUTPUT); digitalWrite(platform_heat, false);
}

#define MIN_TEMP_INTERVAL 20
void Temperature::update()
{
  unsigned long now = millis();
  static unsigned long lasttime = now;
  if(now - lasttime > MIN_TEMP_INTERVAL)
  {
    static bool checkwhich=false;
    if(checkwhich)
      hotend_therm.update();
    else
      platform_therm.update();
    lasttime = now;
    checkwhich = !checkwhich;
  }


  if(!(hotend_heat < 0))
  {
    if(hotend_therm.getTemperature() >= hotend_setpoint)
    {
      digitalWrite(hotend_heat, false);
    }
    else
    {
      digitalWrite(hotend_heat, true);
    }
  }

  if(!(platform_heat < 0))
  {
    if(platform_therm.getTemperature() >= platform_setpoint)
    {
      digitalWrite(platform_heat, false);
    }
    else
    {
      digitalWrite(platform_heat, true);
    }
  }

  doreport();

}

bool Temperature::setHotend(uint16_t temp)
{
  hotend_setpoint = temp;
  return true;
}

bool Temperature::setPlatform(uint16_t temp)
{
  platform_setpoint = temp;
  return true;
}

uint16_t Temperature::getHotend()
{
  return hotend_therm.getTemperature();
}

uint16_t Temperature::getPlatform()
{
  return platform_therm.getTemperature();
}

uint16_t Temperature::getHotendST()
{
  return hotend_setpoint;
}

uint16_t Temperature::getPlatformST()
{
  return platform_setpoint;
}
