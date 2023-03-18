#ifndef FAN_
#define FAN_

#include "config.h"
#include <pwm_defs.h>
#include <pwm_lib.h>

void init_fan();
void set_fan_dutycycle(uint16_t temp);

#endif