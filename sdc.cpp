#include "sdc.h"

void sdc_open() {
  digitalWrite(BMS_FAULT_PIN, LOW);
  digitalWrite(LED_0_PIN, HIGH);
  digitalWrite(AIR_2_EN_PIN, LOW);
}

void sdc_close() {
  digitalWrite(BMS_FAULT_PIN, HIGH);
  digitalWrite(LED_0_PIN, LOW);
  digitalWrite(AIR_2_EN_PIN, HIGH);
}