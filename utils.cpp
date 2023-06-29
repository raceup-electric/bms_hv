#include "utils.h"
#include "operations.h"

uint16_t parse_temp(uint16_t volt) {
  return (uint16_t) ( 
    TEMP_FIT_COEFF[0] * pow(volt, (-TEMP_FIT_COEFF[1])) +
    TEMP_FIT_COEFF[2]
  );
}