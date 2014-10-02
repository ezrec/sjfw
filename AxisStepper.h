#ifndef __AXIS_STEPPER_H__ 
#define __AXIS_STEPPER_H__
/* Axis control class
 * (c) 2011 Christopher "ScribbleJ" Jansen
 *
 */

#include "config.h"
#include <math.h>

#include "Axis.h"

class AxisStepper : public Axis
{
  public:
  AxisStepper(int step_pin, int dir_pin, int enable_pin, int min_pin, int max_pin, 
           float steps_per_unit, bool dir_inverted, 
           float max_feedrate, float home_feedrate, float min_feedrate, 
           float accel_rate_in_units, bool disable_after_move)
           : Axis(min_pin, max_pin, steps_per_unit, dir_inverted,
                  max_feedrate, home_feedrate, min_feedrate,
                  accel_rate_in_units, disable_after_move),
             step_pin(step_pin), dir_pin(dir_pin), enable_pin(enable_pin)
  {
    // Initialize pins we control.
    if(!(step_pin < 0)) { pinMode(step_pin, OUTPUT); }
    if(!(dir_pin < 0))  { pinMode(dir_pin, OUTPUT); }
    if(!(enable_pin < 0)) { pinMode(enable_pin, OUTPUT); }
  }
    
  void  disable() { if(!(enable_pin < 0)) digitalWrite(enable_pin, true); }
  void  enable() { if(!(enable_pin < 0)) digitalWrite(enable_pin, false); }

  int32_t doOneStep()
  {
    digitalWrite(step_pin, true);
    digitalWrite(step_pin, false);

    Axis::doOneStep();

    return getDir() ? 1 : -1;
  }

  bool setupMove(float supposed_position, bool dir, uint32_t steps)
  {
    bool valid;
    
    valid = Axis::setupMove(supposed_position, dir, steps);
    if (!valid)
        return false;

    digitalWrite(dir_pin, getDir());
    return true;
  }  

  void changepinStep(int pin)
  {
    step_pin = pin;
    if((step_pin < 0))
      return;

    pinMode(step_pin, OUTPUT);
    digitalWrite(step_pin, 0);
  }

  void changepinDir(int pin)
  {
    dir_pin = pin;
    if((dir_pin < 0))
      return;

    pinMode(dir_pin, OUTPUT);
    digitalWrite(dir_pin, 0);
  }

  void changepinEnable(int pin)
  {
    enable_pin = pin;
    if((enable_pin < 0))
      return;

    pinMode(enable_pin, OUTPUT);
    digitalWrite(enable_pin, 0);
  }

  bool isInvalid() { return (step_pin < 0); }
  void reportConfigStatus(Host& h)
  {
    if((step_pin < 0))
      h.write_P(PSTR(" no step "));
    if((dir_pin < 0))
      h.write_P(PSTR(" no dir "));
    if((enable_pin < 0))
      h.write_P(PSTR(" no enable "));
  }

private:
  int step_pin;
  int dir_pin;
  int enable_pin;

};

#endif // __AXIS_STEPPER_H__
