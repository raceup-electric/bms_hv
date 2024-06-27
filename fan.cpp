#include "fan.h"

void init_fan() {
  g_bms.fan.on = true;
  pinMode(FAN1_EN_PIN, OUTPUT);
  pinMode(FAN2_EN_PIN, OUTPUT);
  delay(10);
}

void set_fan_dutycycle() {
  if (!g_bms.fan.on) { 
    g_bms.fan.prev_temp = g_bms.max_temp;
    analogWrite(FAN1_EN_PIN, 255);
    analogWrite(FAN2_EN_PIN, 255);
    return;
  }
  float m = (MAX_FAN_SPEED - MIN_FAN_SPEED) / (MAX_TEMP_FAN - MIN_TEMP_FAN);
  float q = MAX_FAN_SPEED - (MAX_TEMP_FAN * m);
  float fan_speed = 0.0;
  if (g_bms.max_temp > MAX_TEMP_FAN) {
    analogWrite(FAN1_EN_PIN, 255 * (1 - MAX_FAN_SPEED));
    analogWrite(FAN2_EN_PIN, 255 * (1 - MAX_FAN_SPEED));
    return;
  }
  if (g_bms.max_temp < MIN_TEMP_FAN) {
    analogWrite(FAN1_EN_PIN, 255 * (1 - MIN_FAN_SPEED));
    analogWrite(FAN2_EN_PIN, 255 * (1 - MIN_FAN_SPEED));
    return;
  }
  fan_speed = m * g_bms.max_temp + q;
  analogWrite(FAN1_EN_PIN, 255 * (1 - fan_speed));
  analogWrite(FAN2_EN_PIN, 255 * (1 - fan_speed));
  g_bms.fan.prev_temp = g_bms.max_temp;
}

// void toggle_fan(BytesUnion* data) {
//   if (data->byte[0] == 0x1) g_bms.fan.on = !g_bms.fan.on;
// }