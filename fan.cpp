#include "fan.h"

void init_fan() {
  pinMode(FAN1_EN_PIN, OUTPUT);
  pinMode(FAN2_EN_PIN, OUTPUT);
  delay(10);
  g_bms.fan.state = FanState::OFF;
}

void set_fan_dutycycle() {
  static uint32_t hv_on_ts = 0;
  float fan_speed = 0.0f;

  if (g_bms.precharge.done && g_bms.fan.state == FanState::OFF) {
      hv_on_ts = millis();
      g_bms.fan.state = FanState::RAMPING;
  }
  else if (g_bms.fan.state == FanState::RAMPING) {
    uint32_t elapsed = (millis() - hv_on_ts) / 1000; // s elapsed since hv on
    fan_speed = (elapsed - FAN_ON_DELAY >= 0) ? 0.1 * (elapsed - FAN_ON_DELAY) : 0.0;
    if (elapsed - FAN_ON_DELAY > 5) {
      g_bms.fan.state = FanState::ON;
    }
  }
  else if (g_bms.fan.state == FanState::ON) {
    float m = (MAX_FAN_SPEED - MIN_FAN_SPEED) / (MAX_TEMP_FAN - MIN_TEMP_FAN);
    float q = MAX_FAN_SPEED - (MAX_TEMP_FAN * m);
    if (g_bms.max_temp > MAX_TEMP_FAN) {
      fan_speed = MAX_FAN_SPEED;
    }
    else if (g_bms.max_temp < MIN_TEMP_FAN) {
      fan_speed = MIN_FAN_SPEED;
    }
    else {
      fan_speed = m * g_bms.max_temp + q;
    }
  }
  analogWrite(FAN1_EN_PIN, 255 * (1 - fan_speed));
  analogWrite(FAN2_EN_PIN, 255 * (1 - fan_speed));
  g_bms.fan.speed = (uint8_t)(fan_speed * 100);
}