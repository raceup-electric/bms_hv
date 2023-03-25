#include "sdc.h"

void sdc_open() {
  digitalWrite(BMS_FAULT_PIN, HIGH);
  digitalWrite(LED_0_PIN, HIGH);
}