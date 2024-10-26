#ifndef CANC
#define CANC

#include "esp32_can.h"
#include "config.h"
#include "lem.h"
#include "precharge.h"
#include "fan.h"

void init_can();
void read_lem(CAN_FRAME *frame);
void read_precharge(CAN_FRAME *frame);
//void read_fan(CAN_FRAME *frame);

void send_data_to_ECU(uint16_t max_volt, uint16_t mean_volt, uint16_t min_volt, uint8_t soc, uint16_t max_temp, uint16_t mean_temp, uint16_t min_temp, uint8_t fan_speed);

#endif