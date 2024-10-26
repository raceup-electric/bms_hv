#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

constexpr float TEMP_FIT_COEFF[3] = {
  1.93252786e+03, //a
  2.42505056e-02, //b
  -1.52425977e+03 //c
  // 56.352829,        // a
  // -161.4534,      // b
  // 120.304777       // c
};

constexpr float KEL2CEL = 273.15;
constexpr uint16_t RTHERMISTOR = 22000;   // Datasheet NTCAFLEX05103HH
constexpr uint16_t VREF2 = 30000;         // Datasheet NTCAFLEX05103HH
constexpr uint16_t B = 3960;               // Datasheet NTCAFLEX05103HH

uint16_t parse_temp(uint16_t volt);

uint16_t bitmap_alive_slaves();

uint8_t n_alive_slaves();

#endif // UTILS_H_