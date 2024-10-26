#ifndef SDC_H_
#define SDC_H_

#include <Arduino.h>
#include "config.h"
#include "structs.h"

extern BMS g_bms;

void sdc_open();

void sdc_close();

#endif // SDC_H_