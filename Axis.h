#ifndef __AXIS_H__ 
#define __AXIS_H__
/* Axis control class
 * (c) 2011 Christopher "ScribbleJ" Jansen
 *
 */

#include "config.h"
#include <math.h>
#include <stdint.h>

#include <Arduino.h>
#include <Stream.h>

#include "Host.h"

class Axis
{
  public:
  Axis(    int min_pin, int max_pin, float steps_per_unit, bool dir_inverted, 
           float max_feedrate, float home_feedrate, float min_feedrate, 
           float accel_rate_in_units, bool disable_after_move)
  {
    // Initialize class data
    this->min_pin = min_pin;
    this->max_pin = max_pin;
    this->steps_per_unit = steps_per_unit;
    this->spu_int = steps_per_unit;
    this->disable_after_move = disable_after_move;
    max_feed     = max_feedrate;
    start_feed   = min_feedrate;
    accel_rate = accel_rate_in_units;
    position = 0;
    this->dir_inverted = dir_inverted;
    steps_to_take = 0;
    steps_remaining = 0;
    minstop_pos = 10000;
    maxstop_pos = 10000;

    // Unused parameters
    (void)home_feedrate;

    // Initialize pins we control.
    if (min_pin >= 0)
        pinMode(min_pin, INPUT_PULLUP);
    if (max_pin >= 0)
        pinMode(max_pin, INPUT_PULLUP);
  }
    
  void dump_to_host()
  {
    HOST.labelnum("p:",position,false);
    HOST.labelnum(" sf:", start_feed, false);
    HOST.labelnum(" mf:", max_feed, false);
    HOST.labelnum(" ar:",accel_rate, false);
    HOST.labelnum(" stt:",steps_to_take);
  }
  // Interval measured in clock ticks
  // feedrate in mm/min
  // F_CPU is clock ticks/second
  uint32_t interval_from_feedrate(float feedrate)
  {
    float a = (float)(F_CPU * 60.0f) / (feedrate * steps_per_unit);
    if(a>F_CPU) a = F_CPU;
    return a;
  }

  inline uint32_t int_interval_from_feedrate(uint32_t feedrate)
  {
    // Max error - roughly 2.5%.  Only used for accel so not really a problem.
    uint32_t a = ((uint32_t)F_CPU * 60) / (feedrate * spu_int);
    if(a>F_CPU) a = F_CPU;
    return a;
  }

	bool isMoving() { return (steps_remaining > 0); };
  // Doesn't take into account position is not updated during move.
  float getCurrentPosition() { return position; }
  void  setCurrentPosition(float pos) { position = pos; }
  void  setMinimumFeedrate(float feedrate) { if(feedrate <= 0) return; start_feed = feedrate; }
  void  setMaximumFeedrate(float feedrate) { if(feedrate <= 0) return; max_feed = feedrate;  }
  void  setAverageFeedrate(float feedrate) { if(feedrate <= 0) return;  }
  void  setStepsPerUnit(float steps) { if(steps <= 0) return; steps_per_unit = steps; spu_int = steps; }
  void  setAccel(float rate) { if(rate <= 0) return; accel_rate = rate; }
  float getAccel() { return accel_rate; }
  void  disable() { }
  void  enable() { }

  float getMovesteps(float start, float end, bool& dir) 
  { 
    float d = end - start; 

    if(d<0) 
    {
      d = d * -1; 
      dir=false;
    }
    else
      dir=true;
    
    return steps_per_unit * d; 
  }
  float    getStartFeed(float feed) { return start_feed < feed ? start_feed : feed; }
  float    getStartFeed() { return start_feed; }
  float    getEndFeed(float feed) { return max_feed < feed ? max_feed : feed; }
  float    getMaxFeed() { return max_feed; }
  uint32_t getStartInterval(float feed) { uint32_t i = interval_from_feedrate(feed); return i; }
  uint32_t getEndInterval(float feed) { uint32_t i = interval_from_feedrate(feed); return i ; }
  uint32_t getAccelRate() { return accel_rate; }

  static float getAccelTime(float startfeed, float endfeed, uint32_t accel)
  { 
    startfeed /= 60.0f;
    endfeed   /= 60.0f;
    return (float)(endfeed - startfeed) / (float)accel;
  }
  int32_t getTimePerAccel() { return ((float)1 / (float)((accel_rate * steps_per_unit) / ((float)F_CPU))); }





  uint32_t getAccelDist(float start_feed, float end_feed, float accel) 
  { 
    end_feed /= 60.0f;
    start_feed /= 60.0f;
    float distance_in_mm = ((end_feed * end_feed) - (start_feed * start_feed)) / (2.0f * accel);
    return distance_in_mm * steps_per_unit;
  }
  
  static float getFinalVelocity(float start_feed, float dist, float accel)
  {
    start_feed /= 60.0f;
    return sqrt((start_feed * start_feed) + (2.0f * accel * dist)) * 60.0f;
  }

  float getSpeedAtEnd(float start_feed, float accel, uint32_t movesteps)
  {
    start_feed /= 60.0f;
    return  sqrt((start_feed * start_feed) + (2.0f * accel * (float)((float)movesteps / steps_per_unit))) * 60.0f;
  }

  float getEndpos(float start, uint32_t steps, bool dir) 
  { 
    return start + (float)((float)steps / steps_per_unit * (dir ? 1.0 : -1.0));
  }

  int doOneStep()
  {
    return 1;
  }

  inline void doStep()
  {
    if(steps_remaining == 0) return;
    if(direction)
    {
      if(!(max_pin < 0) && digitalRead(max_pin) != END_INVERT)
      {
        if(maxstop_pos < 9000)
          position = maxstop_pos;
        else
          position += (float)(steps_to_take-steps_remaining) / steps_per_unit;
        steps_remaining = 0;
        return;
      }
    }
    else
    {
      if(!(min_pin < 0) && digitalRead(min_pin) != END_INVERT)
      {
        if(minstop_pos < 9000)
          position = minstop_pos;
        else
          position -= (float)(steps_to_take-steps_remaining) / steps_per_unit;
        steps_remaining = 0;
        return;
      }
    }

    steps_remaining -= doOneStep();

    if(steps_remaining == 0)
    {
      //HOST.labelnum("FINISH MOVE, ", steps_to_take, true);
      position += (float)((float)steps_to_take / steps_per_unit * (direction ? 1.0 : -1.0));
      //if(disable_after_move) disable();
    }
  }

  bool setupMove(float supposed_position, bool dir, uint32_t steps)
  {
    if(supposed_position != position)
      return false;
    if(steps != 0) enable();
    direction = dir;
    steps_to_take = steps;
    steps_remaining = steps;
    return true;
  }  

  uint32_t getRemainingSteps() { return steps_remaining; }

  void disableIfConfigured() { if(disable_after_move) disable(); }

  void changepinStep(uint8_t pin)
  {
  }

  void changepinDir(uint8_t pin)
  {
  }

  void changepinEnable(uint8_t pin)
  {
  }


  void changepinMin(uint8_t pin)
  {
    min_pin = pin;

    pinMode(pin, INPUT_PULLUP);
  }

  void changepinMax(uint8_t pin)
  {
    max_pin = pin;

    pinMode(pin, INPUT_PULLUP);
  }

  void setMinStopPos(float v) { minstop_pos = v; }
  void setMaxStopPos(float v) { maxstop_pos = v; }

  void setInvert(bool v) { dir_inverted = v; }
  void setDisable(bool v) { disable_after_move = v; }
  static void setPULLUPS(bool v) { PULLUPS = v; };
  static void setEND_INVERT(bool v) { END_INVERT = v; };

  

  bool isInvalid() { return true; }
  void reportConfigStatus(Host& h)
  {
    if(steps_per_unit == 1)
      h.write(" no steps_per_unit ");
    if(accel_rate == 1)
      h.write(" no accel ");
    if((min_pin < 0))
      h.write(" no min ");
    if((max_pin < 0))
      h.write(" no max ");
    if(disable_after_move)
      h.write(" DIS ");
    if(dir_inverted)
      h.write(" INV ");
    if(PULLUPS)
      h.write(" EPULL ");
    if(END_INVERT)
      h.write(" EINV ");
  }
  float getStepsPerMM() { return steps_per_unit; }

  bool getDir() { return direction ^ dir_inverted; }

private:
  static bool PULLUPS;
  static bool END_INVERT;
  volatile float position;
  volatile bool direction;
  volatile uint32_t steps_to_take;
  volatile uint32_t steps_remaining;

  float steps_per_unit;
  uint32_t spu_int;

  float    start_feed, max_feed;
  uint32_t accel_rate;

  int homing_dir;
  // Automatically set positions when hitting endstops.
  float minstop_pos;
  float maxstop_pos;

  bool dir_inverted;
  int min_pin;
  int max_pin;
  bool disable_after_move;

};

#endif // __AXIS_H__
