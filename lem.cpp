#include "lem.h"

void set_lem(BytesUnion *data) {
  // uint32_t current = 0;
  // current |= ((uint32_t)(data->bytes[0]) << 24);
  // current |= ((uint32_t)(data->bytes[1]) << 16);
  // current |= ((uint32_t)(data->bytes[2]) << 8);
  // current |= ((uint32_t)(data->bytes[3]) << 0);
  // current ^= 1 << 31;
  // g_bms.lem.curr = (int32_t)(current);
  // g_bms.lem.last_recv = millis();
  uint32_t current = 0;
  uint8_t tmp = data->bytes[0];
  tmp ^= 1<<7;
  current |= ((tmp) << 24);
  current |= ((data->bytes[1]) << 16);
  current |= ((data->bytes[2]) << 8);
  current |= ((data->bytes[3]) << 0);
  g_bms.lem.curr = -(int32_t)(current);
  g_bms.lem.last_recv = millis();
}

/*
 * If LEM message is not received after LEM_TIMEOUT ms, send fault
 */
bool is_lem_in_time(){
  return ((millis() - g_bms.lem.last_recv) < LEM_TIMEOUT);
}
