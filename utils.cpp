#include "utils.h"
#include "operations.h"

uint16_t parse_temp(uint16_t volt) {
  // se non va chiedere a Cracco e Vardabasso
  uint16_t R = (RTHERMISTOR * volt) / (VREF2 - volt);
  return (uint16_t) (
    B/(log(58.65 * R)) - KEL2CEL
  );
  //return (uint16_t) ( 
  //  TEMP_FIT_COEFF[0] * pow(volt, (-TEMP_FIT_COEFF[1])) +
  //  TEMP_FIT_COEFF[2]
  //);
}

uint16_t bitmap_alive_slaves() {
  uint16_t bitmap = 0;
  for (int i = 0; i < SLAVE_NUM; i++) {
    if (g_bms.slaves[i].err >= MIN_ERR_THRESHOLD) bitmap |= (1 << i);
  }
  return ~bitmap;
}

uint8_t n_alive_slaves() {
  uint8_t alives = 0;
  for (int i = 0; i < SLAVE_NUM; i++) {
    if (g_bms.slaves[i].err < MIN_ERR_THRESHOLD) alives++;
  }
  return alives;
}