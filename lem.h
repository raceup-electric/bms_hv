#ifndef BMS_LEMREADER_H
#define BMS_LEMREADER_H

#include "config.h"
#include "structs.h"
#include "canc.h"

constexpr uint16_t LEM_TIMEOUT = 500;

extern BMS g_bms;

void set_lem(BytesUnion *data);
bool is_lem_in_time();

#endif
