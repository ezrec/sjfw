#ifndef __AXIS_ENCODER_H__ 
#define __AXIS_ENCODER_H__
/* Axis control class
 * (c) 2014 Jason S. McMullan
 *
 */

#include "config.h"
#include "Host.h"
#include "AvrPort.h"
#include <math.h>

#include <AFMotor.h>
#include <Encoder.h>

#include "Axis.h"

class AxisEncoder : public Axis
{
  public:
  AxisEncoder(uint8_t motor_select, uint8_t encoder_a, uint8_t encoder_b, Pin min_pin, Pin max_pin, 
           float steps_per_unit, bool dir_inverted, 
           float max_feedrate, float home_feedrate, float min_feedrate, 
           float accel_rate_in_units, bool disable_after_move)
           : Axis(min_pin, max_pin, steps_per_unit, dir_inverted,
                  max_feedrate, home_feedrate, min_feedrate,
                  accel_rate_in_units, disable_after_move),
             motor(motor_select), encoder(encoder_a, encoder_b)
  {
    // Initialize pins we control.
    motor.setSpeed(0);
    motor.run(RELEASE);
  }
    
  void  disable() { motor.setSpeed(0); motor.run(RELEASE); }
  void  enable() { }

  int32_t doOneStep()
  {
    int32_t delta = (encoder.read() - encoder_last);
    return getDir() ? delta : -delta;
  }

  bool setupMove(float supposed_position, bool dir, uint32_t steps)
  {
    bool valid;
    
    valid = Axis::setupMove(supposed_position, dir, steps);
    if (!valid)
        return false;

    motor.setSpeed(getStartFeed());
    motor.run(getDir() ? FORWARD : BACKWARD);
    return true;
  }  

  bool isInvalid() { return false; }
  void reportConfigStatus(Host& h)
  {
    (void)h; // unused
  }

private:
  int32_t encoder_last;
  float max_feedrate;
  float min_feedrate;
  AF_DCMotor motor;
  Encoder encoder;

};

#endif // __AXIS_ENCODER_H__
