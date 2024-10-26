#include "sdc.h"

void sdc_open() {
  digitalWrite(BMS_FAULT_PIN, LOW);
  digitalWrite(LED_0_PIN, HIGH);
  digitalWrite(AIR_2_EN_PIN, LOW);
  g_bms.precharge.done = false;
}

void sdc_close() {
  digitalWrite(BMS_FAULT_PIN, HIGH);
  digitalWrite(LED_0_PIN, LOW);
}