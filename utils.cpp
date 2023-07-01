#include "utils.h"
#include "operations.h"

uint16_t parse_temp(uint16_t volt) {
  return (uint16_t) ( 
    TEMP_FIT_COEFF[0] * pow(volt, (-TEMP_FIT_COEFF[1])) +
    TEMP_FIT_COEFF[2]
  );
}

uint16_t alive_slaves() {
  uint16_t bitmap = 0;
  for (int i = 0; i < SLAVE_NUM; i++) {
    if (g_bms.slaves[i].err > MIN_ERR_THRESHOLD) bitmap |= (1 << i);
  }
  return ~bitmap;
}