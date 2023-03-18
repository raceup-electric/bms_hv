#include "precharge.h"

void set_precharge(BytesUnion* data) {
  prech.bus_volt = (data->bytes[0] | data->bytes[1] << 8);
  prech.via_can = true;
}