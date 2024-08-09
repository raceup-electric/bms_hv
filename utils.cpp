#include "utils.h"
#include "operations.h"

uint16_t parse_temp(uint16_t volt) {
  // se non va chiedere a Cracco e Vardabasso
  // Consider USHRT_MAX as a fault value because it would trigger the overtemp condition
  constexpr uint16_t MAX_TEMP = USHRT_MAX;
  constexpr uint16_t MIN_TEMP = 0;
  if (volt <= 0) return MAX_TEMP; // if voltage is zero (below zero is impossible) => thermistor shorted to ground so return max temperature 
  if ((VREF2 - volt) <= 0) return MIN_TEMP; // VREF - volt == 0 (below zero is impossible) => thermistor is disconnected 
  uint16_t R = (RTHERMISTOR * volt) / (VREF2 - volt);
  uint16_t temp = (uint16_t) (B/(log(58.65 * R)) - KEL2CEL);
  return temp < MIN_TEMP ? 0 : temp > MAX_TEMP ? MAX_TEMP : temp; // Saturate temperature
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