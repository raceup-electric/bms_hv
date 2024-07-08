#include "utils.h"
#include "operations.h"

uint16_t parse_temp(uint16_t volt) {
  // se non va chiedere a Cracco e Vardabasso
  // Consider USHRT_MAX as a fault value because it would trigger the overtemp condition
  constexpr uint16_t FAULT = USHRT_MAX;
  if (volt == 0) return FAULT; // if voltage is zero => thermistor disconnected so return fault
  if ((VREF2 - volt) == 0) return FAULT; // VREF - volt == 0 => thermistor is shorted to VREF so return fault 
  uint16_t R = (RTHERMISTOR * volt) / (VREF2 - volt);
  if (R <= 0) return USHRT_MAX;
  uint16_t temp = (uint16_t) (B/(log(58.65 * R)) - KEL2CEL);
  return temp >= 0 ? temp : FAULT; // If temperature is negative return fault
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