#ifndef LEM_H_
#define LEM_H_

#include "config.h"
#include "structs.h"
#include "canc.h"

constexpr uint16_t LEM_TIMEOUT = 500;

extern BMS g_bms;

void set_lem(BytesUnion *data);
bool is_lem_in_time();

#endif
