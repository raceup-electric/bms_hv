#include "fan.h"

void init_fan() {
  pinMode(FAN_EN_PIN, OUTPUT);
  delay(10);
}

void set_fan_dutycycle() {
  float m = (MAX_FAN_SPEED - MIN_FAN_SPEED) / (MAX_TEMP_FAN - MIN_TEMP_FAN);
  float q = MAX_FAN_SPEED - (MAX_TEMP_FAN * m);
  float fan_speed = 0.0;
  if (g_bms.max_temp > MAX_TEMP_FAN) {
    analogWrite(FAN_EN_PIN, 255 * MAX_FAN_SPEED);
    if (DEBUG) {
      Serial.println("Max temp: " + String(g_bms.max_temp));
      Serial.println("Fan speed: " + String(MAX_FAN_SPEED));
    }
    g_bms.prev_max_temp = g_bms.max_temp;
    return;
  }
  if (g_bms.max_temp < MIN_TEMP_FAN) {
    analogWrite(FAN_EN_PIN, 255 * MIN_FAN_SPEED);
    if (DEBUG) {
      Serial.println("Max temp: " + String(g_bms.max_temp));
      Serial.println("Fan speed: " + String(MIN_FAN_SPEED));
    }
    g_bms.prev_max_temp = g_bms.max_temp;
    return;
  }
  if ((g_bms.max_temp - g_bms.prev_max_temp) > VAR_MIN || (g_bms.max_temp - g_bms.prev_max_temp) < -VAR_MIN) {
    fan_speed = m * g_bms.max_temp + q;
    analogWrite(FAN_EN_PIN, 255 * fan_speed);
    if (DEBUG) {
      Serial.println("Prev max temp: " + String(g_bms.prev_max_temp));
      Serial.println("Max temp: " + String(g_bms.max_temp));
      Serial.println("Fan speed: " + String(fan_speed));
    }
    g_bms.prev_max_temp = g_bms.max_temp;
  }
}