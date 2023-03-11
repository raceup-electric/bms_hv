#include "utils.h"

uint16_t parse_temp(uint16_t volt) {
  float temp = volt / 10000.0;
  return (uint16_t) ( 
    TEMP_FIT_COEFF[0] * pow(temp, 2) +
    TEMP_FIT_COEFF[1] * temp + 
    TEMP_FIT_COEFF[2]
  );
}