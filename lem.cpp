#include "lem.h"

void set_lem(BytesUnion *data) {
  uint32_t current = 0;
  current |= ((uint32_t)(data->bytes[0]) << 24);
  current |= ((uint32_t)(data->bytes[1]) << 16);
  current |= ((uint32_t)(data->bytes[2]) << 8);
  current |= ((uint32_t)(data->bytes[3]) << 0);
  current ^= 1 << 31;
  lem.curr = (int32_t)(current);
  lem.last_recv = millis();
}

/*
 * If LEM message is not received after 500 ms, send fault
 */
bool is_lem_in_time(){
  return !((millis() - lem.last_recv) > LEM_TIMEOUT);
}
