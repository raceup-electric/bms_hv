#ifndef PRECHARGE_H_
#define PRECHARGE_H_

#include "canc.h"

extern BMS g_bms;

void set_precharge(BytesUnion *data);

void precharge_control();

#endif // PRECHARGE_H_