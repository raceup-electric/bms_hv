#ifndef BMS_LEMREADER_H
#define BMS_LEMREADER_H

#include "config.h"
#include "canc.h"

constexpr uint16_t LEM_TIMEOUT = 500;

struct LEM {
  uint32_t curr;
  uint32_t last_recv;
};

extern LEM lem;

void set_lem(BytesUnion *data);
bool is_lem_in_time();

#endif
