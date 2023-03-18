#include "utils.h"
#include "operations.h"

uint16_t parse_temp(uint16_t volt) {
  float temp = volt / 10000.0;
  return (uint16_t) ( 
    TEMP_FIT_COEFF[0] * pow(temp, 2) +
    TEMP_FIT_COEFF[1] * temp + 
    TEMP_FIT_COEFF[2]
  );
}

void minMaxAvg_Volts(uint16_t* voltsArray) {
  uint16_t minVolt = 0xFFFF;
  uint16_t maxVolt = 0x0;
  uint32_t meanVolt = 0x0;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex++) {
      uint16_t cellVolt = slaves[slaveIndex].volts[cellIndex];

      if(cellVolt < minVolt) minVolt = cellVolt;
      if(cellVolt > maxVolt) maxVolt = cellVolt;

      meanVolt += cellVolt;
    }
  }

  voltsArray[0] = minVolt;
  voltsArray[1] = maxVolt;
  voltsArray[2] = meanVolt / (SLAVE_NUM * CELL_NUM);
}

void minMaxAvg_Temps(uint16_t* tempsArray) {
  uint16_t minTemp = 0xFFFF;
  uint16_t maxTemp = 0x0;
  uint8_t maxTempSlave = 0x0;

  uint32_t meanTemp = 0x0;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex++) {
      uint16_t cellTemp = slaves[slaveIndex].temps[(int) cellIndex/3];

      if(cellTemp < minTemp) minTemp = cellTemp;
      if(cellTemp > maxTemp) {
        maxTemp = cellTemp;
        maxTempSlave = slaveIndex;
      }

      meanTemp += cellTemp;
    }
  }

  tempsArray[0] = minTemp;
  tempsArray[1] = maxTemp;
  tempsArray[2] = meanTemp / (SLAVE_NUM * CELL_NUM);
  tempsArray[3] = maxTempSlave;
}