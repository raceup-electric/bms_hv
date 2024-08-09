#include "canc.h"
#include "utils.h"

void init_can() {
  Can0.setCANPins((gpio_num_t)CAN_RX_PIN, (gpio_num_t)CAN_TX_PIN);
  Can0.begin(CAN_BAUD_RATE);
  Can0.setRXBufferSize(30);
  // Can1.begin(CAN_BAUD_RATE);
  int bus_arrived = Can0.setRXFilter(DC_BUS_VOLTAGE_ID, 0x7FF, false);
  int lem_arrived = Can0.setRXFilter(LEM_CURRENT_ID, 0x7FF, false);
  //int toggle_fan_arrived = Can0.setRXFilter(FAN_TOGGLE_ID, 0x7FF, false);
  
  Can0.setCallback(bus_arrived, read_precharge);
  Can0.setCallback(lem_arrived, read_lem);
  //Can0.setCallback(toggle_fan_arrived, read_fan);
}

void read_lem(CAN_FRAME *frame) {
  set_lem(&(frame->data));
}

void read_precharge(CAN_FRAME *frame) {
  set_precharge(&(frame->data));
}

//void read_fan(CAN_FRAME *frame) {
//  toggle_fan(&(frame->data));
//}

void send_data_to_ECU(uint16_t max_volt, uint16_t mean_volt, uint16_t min_volt, uint8_t soc, uint16_t max_temp, uint16_t mean_temp, uint16_t min_temp, uint8_t fan_speed) {
  delay(3);

  constexpr uint8_t VOLT_DATA_SIZE = 7;
  constexpr uint8_t TEMP_DATA_SIZE = 7;

  union __attribute__((packed)) VoltData {
    struct VoltFields {
      uint16_t maxValue;
      uint16_t minValue;
      uint16_t meanValue;
      uint8_t soc;
    } fields;
    uint8_t bytes[VOLT_DATA_SIZE];
  };

  union __attribute__((packed)) TempData {
    struct TempFields {
      uint16_t maxValue;
      uint16_t minValue;
      uint16_t meanValue;
      uint8_t fanSpeed;
    } fields;
    uint8_t bytes[TEMP_DATA_SIZE];
  };


  VoltData volt_data;
  volt_data.fields.maxValue = max_volt;
  volt_data.fields.minValue = min_volt;
  volt_data.fields.meanValue = mean_volt;
  volt_data.fields.soc = soc;
  // Send voltage
  CAN_FRAME outgoingVoltage;
  outgoingVoltage.id = DATA_VOLTAGE_ID;
  outgoingVoltage.length = VOLT_DATA_SIZE;
  for(int i = 0 ; i < VOLT_DATA_SIZE; ++i)
    outgoingVoltage.data.byte[i] = volt_data.bytes[i];
  Can0.sendFrame(outgoingVoltage);

  // Send temperature
  CAN_FRAME outgoingTemperatures;
  outgoingTemperatures.id = DATA_TEMP_ID;
  outgoingTemperatures.length = TEMP_DATA_SIZE;
    
  TempData temp_data;
  temp_data.fields.maxValue = max_temp;
  temp_data.fields.minValue = min_temp;
  temp_data.fields.meanValue = mean_temp;
  temp_data.fields.fanSpeed = fan_speed;
  
  for(int i = 0 ; i < TEMP_DATA_SIZE; ++i)
    outgoingTemperatures.data.byte[i] = temp_data.bytes[i];
  Can0.sendFrame(outgoingTemperatures);
}