#ifndef SOC_H_
#define SOC_H_

#define A_SOC 0.5208288828722637
#define B_SOC 0.025418345162304706 
#define C_SOC 485.0016077065276
#define D_SOC 0.43763791169703486

#include "structs.h"

extern BMS g_bms;

void init_soc();
void estimate_soc();

#endif SOC_H_