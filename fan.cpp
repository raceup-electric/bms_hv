#include "fan.h"

void init_fan() {
  pinMode(FAN1_EN_PIN, OUTPUT);
  pinMode(FAN2_EN_PIN, OUTPUT);
  delay(10);
}

void set_fan_dutycycle() {
  float m = (MAX_FAN_SPEED - MIN_FAN_SPEED) / (MAX_TEMP_FAN - MIN_TEMP_FAN);
  float q = MAX_FAN_SPEED - (MAX_TEMP_FAN * m);
  float fan_speed = 0.0f;
  if (g_bms.max_temp > MAX_TEMP_FAN) {
    fan_speed = MAX_FAN_SPEED;
  }
  else if (g_bms.max_temp < MIN_TEMP_FAN) {
    fan_speed = MIN_FAN_SPEED;
  }
  else {
    fan_speed = m * g_bms.max_temp + q;
  }
  analogWrite(FAN1_EN_PIN, 255 * (1 - fan_speed));
  analogWrite(FAN2_EN_PIN, 255 * (1 - fan_speed));
  g_bms.fan.speed = (uint8_t)(fan_speed * 100);
}