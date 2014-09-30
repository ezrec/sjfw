#ifndef __AXIS_STEPPER_H__ 
#define __AXIS_STEPPER_H__
/* Axis control class
 * (c) 2011 Christopher "ScribbleJ" Jansen
 *
 */

#include "config.h"
#include "Host.h"
#include "AvrPort.h"
#include <math.h>

#include "Axis.h"

class AxisStepper : public Axis
{
  public:
  AxisStepper(Pin step_pin, Pin dir_pin, Pin enable_pin, Pin min_pin, Pin max_pin, 
           float steps_per_unit, bool dir_inverted, 
           float max_feedrate, float home_feedrate, float min_feedrate, 
           float accel_rate_in_units, bool disable_after_move)
           : Axis(min_pin, max_pin, steps_per_unit, dir_inverted,
                  max_feedrate, home_feedrate, min_feedrate,
                  accel_rate_in_units, disable_after_move),
             step_pin(step_pin), dir_pin(dir_pin), enable_pin(enable_pin)
  {
    // Initialize pins we control.
    if(!step_pin.isNull()) { step_pin.setDirection(true); step_pin.setValue(false); }
    if(!dir_pin.isNull())  { dir_pin.setDirection(true); dir_pin.setValue(false); }
    if(!enable_pin.isNull()) { enable_pin.setDirection(true); enable_pin.setValue(true); }
  }
    
  void  disable() { if(!enable_pin.isNull()) enable_pin.setValue(true); }
  void  enable() { if(!enable_pin.isNull()) enable_pin.setValue(false); }

  int32_t doOneStep()
  {
    step_pin.setValue(true);
    step_pin.setValue(false);

    Axis::doOneStep();

    return getDir() ? 1 : -1;
  }

  bool setupMove(float supposed_position, bool dir, uint32_t steps)
  {
    bool valid;
    
    valid = Axis::setupMove(supposed_position, dir, steps);
    if (!valid)
        return false;

    dir_pin.setValue(getDir());
    return true;
  }  

  void changepinStep(Port p, int bit)
  {
    step_pin = Pin(p, bit);
    if(step_pin.isNull())
      return;
    step_pin.setDirection(true); step_pin.setValue(false);
  }

  void changepinDir(Port p, int bit)
  {
    dir_pin = Pin(p, bit);
    if(dir_pin.isNull())
      return;

    dir_pin.setDirection(true); dir_pin.setValue(false);
  }

  void changepinEnable(Port p, int bit)
  {
    enable_pin = Pin(p, bit);
    if(enable_pin.isNull())
      return;

    enable_pin.setDirection(true); enable_pin.setValue(true);
  }

  bool isInvalid() { return step_pin.isNull(); }
  void reportConfigStatus(Host& h)
  {
    if(step_pin.isNull())
      h.write_P(PSTR(" no step "));
    if(dir_pin.isNull())
      h.write_P(PSTR(" no dir "));
    if(enable_pin.isNull())
      h.write_P(PSTR(" no enable "));
  }

private:
  Pin step_pin;
  Pin dir_pin;
  Pin enable_pin;

};

#endif // __AXIS_STEPPER_H__
