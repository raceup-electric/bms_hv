#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

constexpr float TEMP_FIT_COEFF[3] = {
  56.352829,        // x^2
  -161.4534,      // x^1
  120.304777       // x^0
};

uint16_t parse_temp(uint16_t volt);

uint16_t min_volt();

uint16_t max_volt();

uint16_t avg_volt();

uint16_t min_temp();

uint16_t max_temp();

uint16_t avg_temp();

uint8_t max_temp_nslave();

#endif // UTILS_H_