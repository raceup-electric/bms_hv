#include "canc.h"

constexpr uint32_t CAN_BAUD_RATE = 1000000;
constexpr uint32_t DC_BUS_VOLTAGE_ID = 0x120;
constexpr uint32_t LEM_CURRENT_ID = 0x3c2;
constexpr uint32_t DATA_VOLTAGE_ID = 0x110;
constexpr uint32_t DATA_TEMP_ID = 0x111;

void init_can(){
    Can0.begin(CAN_BAUD_RATE);
    Can0.setNumTXBoxes(1);
    int bus_arrived = Can0.setRXFilter(DC_BUS_VOLTAGE_ID, 0x7FF, false);
    int lem_arrived = Can0.setRXFilter(LEM_CURRENT_ID, 0x7FF, false);
    
    Can0.setCallback(lem_arrived, read_lem);
    Can0.setCallback(bus_arrived, read_precharge);
}

void read_lem(CAN_FRAME *frame) {
  set_lem(&(frame->data));
}

void read_precharge(CAN_FRAME *frame) {
  set_precharge(&(frame->data));
}

void send_data_to_ECU(uint16_t max_volt, uint16_t mean_volt, uint16_t min_volt, uint16_t max_temp, uint16_t mean_temp, uint16_t min_temp, uint8_t max_temp_nslave) {
  /*  Serial.print("MAX VOlT:");
  Serial.println(max_volt);
  Serial.print("MIN VOLT: ");
  Serial.println(min_volt);
  Serial.print("MEAN VOLT: ");
  Serial.println(mean_volt);
  */
  unsigned char voltageMsg[8];
  unsigned char temperatureMsg[8];
  struct data {
    uint16_t maxValue;
    uint16_t minValue;
    uint16_t meanValue;
  };
  typedef struct data Data;

  Data* voltageMSG = (Data*) voltageMsg;

  // Send voltage
  CAN_FRAME outgoingVoltage;
  outgoingVoltage.id = DATA_VOLTAGE_ID;
  outgoingVoltage.length = 6;
  voltageMSG -> maxValue = max_volt;
  voltageMSG -> minValue = min_volt;
  voltageMSG -> meanValue = mean_volt;
  
  for(int i = 0 ; i < 8; ++i)
    outgoingVoltage.data.byte[i] = voltageMsg[i];
  Can0.sendFrame(outgoingVoltage);

  delay(2);

  // Send temperature
  CAN_FRAME outgoingTemperatures;
  outgoingTemperatures.id = DATA_TEMP_ID;
  outgoingTemperatures.length = 7;
    
  Data* tempMSG = (Data*) temperatureMsg;
  tempMSG -> maxValue = max_temp;
  tempMSG -> minValue = min_temp;
  tempMSG -> meanValue = mean_temp;
  temperatureMsg[6] = max_temp_nslave;
  
  for(int i = 0 ; i < 8; ++i)
    outgoingTemperatures.data.byte[i] = temperatureMsg[i];
  Can0.sendFrame(outgoingTemperatures);
}