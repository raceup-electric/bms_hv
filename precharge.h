#ifndef PRECHARGE_H_
#define PRECHARGE_H_

#include "canc.h"

struct Precharge {
  float bus_volt;
  bool via_can;
};

extern Precharge prech;

void set_precharge(BytesUnion *data);

#endif // PRECHARGE_H_