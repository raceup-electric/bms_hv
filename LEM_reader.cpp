#include "LEM_reader.h"

constexpr uint16_t LEM_TIMEOUT = 500;
int32_t lem_curr;
unsigned long LEMlastMessage;

/*
 * Simple can message read
 */
void set_LEM(CAN_FRAME *frame) {
  uint32_t reassembled_data = 0;
  reassembled_data |= ((uint32_t)(frame->data.bytes[0]) << 24);
  reassembled_data |= ((uint32_t)(frame->data.bytes[1]) << 16);
  reassembled_data |= ((uint32_t)(frame->data.bytes[2]) << 8);
  reassembled_data |= ((uint32_t)(frame->data.bytes[3]) << 0);
  reassembled_data ^= 1 << 31;
  lem_curr = (int32_t)(reassembled_data);
  LEMlastMessage = millis();

  #ifdef DEBUG_SENDYNE
      Serial.print("Current [mA]: ");
      Serial.println(lem_curr);
      Serial.println(frame->data.bytes[4],HEX);
  #endif
}

/*
 * If LEM message is not received after 500 ms, send fault
 */
bool isLEMInTime(){
  return !((millis() - LEMlastMessage) > LEM_TIMEOUT);
}
