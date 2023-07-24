#include "canc.h"
#include "utils.h"

void init_can(){
    Can0.begin(CAN_BAUD_RATE);
    Can0.setNumTXBoxes(2);
    // Can1.begin(CAN_BAUD_RATE);
    int bus_arrived = Can0.setRXFilter(DC_BUS_VOLTAGE_ID, 0x7FF, false);
    int lem_arrived = Can0.setRXFilter(LEM_CURRENT_ID, 0x7FF, false);
    int toggle_fan_arrived = Can0.setRXFilter(FAN_TOGGLE_ID, 0x7FF, false);
    
    Can0.setCallback(bus_arrived, read_precharge);
    Can0.setCallback(lem_arrived, read_lem);
    Can0.setCallback(toggle_fan_arrived, read_fan);
}

void read_lem(CAN_FRAME *frame) {
  set_lem(&(frame->data));
}

void read_precharge(CAN_FRAME *frame) {
  set_precharge(&(frame->data));
}

void read_fan(CAN_FRAME *frame) {
  toggle_fan(&(frame->data));
}

void send_data_to_ECU(uint16_t max_volt, uint16_t mean_volt, uint16_t min_volt, uint16_t alive_slaves, uint16_t max_temp, uint16_t mean_temp, uint16_t min_temp, uint8_t max_temp_nslave) {
  delay(50);
  unsigned char voltageMsg[8];
  unsigned char temperatureMsg[8];

  struct VoltData {
    uint16_t maxValue;
    uint16_t minValue;
    uint16_t meanValue;
    uint16_t alives;
  };
  struct TempData {
    uint16_t maxValue;
    uint16_t minValue;
    uint16_t meanValue;
  };

  VoltData* voltageMSG = (VoltData*) voltageMsg;

  // Send voltage
  CAN_FRAME outgoingVoltage;
  outgoingVoltage.id = DATA_VOLTAGE_ID;
  outgoingVoltage.length = 8;
  voltageMSG -> maxValue = max_volt;
  voltageMSG -> minValue = min_volt;
  voltageMSG -> meanValue = mean_volt;
  voltageMSG -> alives = alive_slaves;
  
  for(int i = 0 ; i < 8; ++i)
    outgoingVoltage.data.byte[i] = voltageMsg[i];
  Can0.sendFrame(outgoingVoltage);

  delay(2);

  // Send temperature
  CAN_FRAME outgoingTemperatures;
  outgoingTemperatures.id = DATA_TEMP_ID;
  outgoingTemperatures.length = 7;
    
  TempData* tempMSG = (TempData*) temperatureMsg;
  tempMSG -> maxValue = max_temp;
  tempMSG -> minValue = min_temp;
  tempMSG -> meanValue = mean_temp;
  temperatureMsg[6] = max_temp_nslave;
  
  for(int i = 0 ; i < 8; ++i)
    outgoingTemperatures.data.byte[i] = temperatureMsg[i];
  Can0.sendFrame(outgoingTemperatures);
}