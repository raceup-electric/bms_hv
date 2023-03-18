#include "fan.h"

using namespace arduino_due::pwm_lib;

pwm<pwm_pin::PWMH0_PC3> pwm_pin35;

void init_fan() {
  pwm_pin35.start(DEFAULT_FAN_PERIOD, DEFAULT_FAN_PERIOD * 0.9);    // start at 10%
  delay(10);
  pwm_pin35.set_duty(DEFAULT_FAN_PERIOD * 0.9);                     // go to 10%
}

void set_fan_dutycycle(uint16_t temp) {
  float newDuty;
  if(temp < MIN_TEMP_FAN || temp > MAX_TEMP_FAN)
    newDuty = (temp < MIN_TEMP_FAN) ? 0 : 1;
  else
    newDuty = (temp - MIN_TEMP_FAN) / (MAX_TEMP_FAN - MIN_TEMP_FAN);
    
  // logica invertita
  pwm_pin35.set_duty(DEFAULT_FAN_PERIOD * (1 - newDuty));
}