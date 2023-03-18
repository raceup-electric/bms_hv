#ifndef SDC_H_
#define SDC_H_

#include <Arduino.h>

constexpr uint8_t BMS_FAULT_PIN = 4;
constexpr uint8_t SDC_SENSE_PIN = 6;
constexpr uint8_t AIR_2_PIN = 5;
constexpr uint8_t LED_0_PIN = 3;

void sdc_open();

#endif // SDC_H_