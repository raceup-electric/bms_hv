#include "fan.h"

void init_fan() {
  pinMode(FAN_EN_PIN, OUTPUT);
  delay(10);
  //analogWrite(FAN_PIN, (uint32_t) MAX_FAN_DUTY * 0.1);        // go to 10%
}

void set_fan_dutycycle() {
  uint16_t temp = g_bms.max_temp;
  float newDuty;
  if(temp < MIN_TEMP_FAN || temp > MAX_TEMP_FAN)
    newDuty = (temp < MIN_TEMP_FAN) ? 0 : 1;
  else
    newDuty = (temp - MIN_TEMP_FAN) / (float)(MAX_TEMP_FAN - MIN_TEMP_FAN);
  analogWrite(FAN_EN_PIN, newDuty * MAX_FAN_DUTY);
}