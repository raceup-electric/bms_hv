#include "sdc.h"

void sdc_open() {
  pinMode(BMS_FAULT_PIN, OUTPUT);
  pinMode(LED_0_PIN, OUTPUT);
  digitalWrite(BMS_FAULT_PIN, HIGH);
  digitalWrite(LED_0_PIN, HIGH);
}