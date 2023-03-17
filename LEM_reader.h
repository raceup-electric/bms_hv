#ifndef BMS_LEMREADER_H
#define BMS_LEMREADER_H
#include "config.h"
//#include "serialManager.h"
#include "canc.h"

extern int32_t lem_curr;
extern unsigned long LEMlastMessage;

void set_LEM(CAN_FRAME *frame);
bool isLEMInTime();

#endif
