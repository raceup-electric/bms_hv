#include "fan.h"

void init_fan() {
  g_bms.fan.on = true;
  pinMode(FAN_EN_PIN, OUTPUT);
  delay(10);
}

void set_fan_dutycycle() {
  if (!g_bms.fan.on) analogWrite(FAN_EN_PIN, 255);
  float m = (MAX_FAN_SPEED - MIN_FAN_SPEED) / (MAX_TEMP_FAN - MIN_TEMP_FAN);
  float q = MAX_FAN_SPEED - (MAX_TEMP_FAN * m);
  float fan_speed = 0.0;
  if (g_bms.max_temp > MAX_TEMP_FAN) {
    analogWrite(FAN_EN_PIN, 255 * (1 - MAX_FAN_SPEED));
    g_bms.fan.prev_temp = g_bms.fan.prev_temp;
    return;
  }
  if (g_bms.max_temp < MIN_TEMP_FAN) {
    analogWrite(FAN_EN_PIN, 255 * (1 - MIN_FAN_SPEED));
    g_bms.fan.prev_temp = g_bms.fan.prev_temp;
    return;
  }
  if ((g_bms.max_temp - g_bms.fan.prev_temp) > VAR_MIN || (g_bms.max_temp - g_bms.fan.prev_temp) < -VAR_MIN) {
    fan_speed = m * g_bms.max_temp + q;
    analogWrite(FAN_EN_PIN, 255 * (1 - fan_speed));
    g_bms.fan.prev_temp = g_bms.max_temp;
  }
}

void toggle_fan(BytesUnion* data) {
  if (data->byte[0] == 0x1) g_bms.fan.on = !g_bms.fan.on; 
}