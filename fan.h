#ifndef FAN_H_
#define FAN_H_

#include "config.h"
#include "structs.h"
#include "canc.h"

extern BMS g_bms;

void init_fan();
void set_fan_dutycycle();
void toggle_fan(BytesUnion* data);

#endif