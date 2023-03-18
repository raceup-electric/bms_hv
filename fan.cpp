#include "fan.h"
#include "utils.h"
#include "operations.h"

constexpr uint8_t FAN_PIN = 32;
constexpr uint32_t MAX_FAN_DUTY = 255; 

void init_fan() {
  pinMode(FAN_PIN, OUTPUT);
  delay(10);
  analogWrite(FAN_PIN, (uint32_t) MAX_FAN_DUTY * 0.1);        // go to 10%
}

void set_fan_dutycycle() {
  uint16_t temp = parse_temp(bms_data.max_temp);
  float newDuty;
  if(temp < MIN_TEMP_FAN || temp > MAX_TEMP_FAN)
    newDuty = (temp < MIN_TEMP_FAN) ? 0 : 1;
  else
    newDuty = (temp - MIN_TEMP_FAN) / (MAX_TEMP_FAN - MIN_TEMP_FAN);
  
  analogWrite(FAN_PIN, (uint32_t) MAX_FAN_DUTY * newDuty);
}