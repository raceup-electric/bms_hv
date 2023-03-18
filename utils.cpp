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

uint16_t min_volt() {
  uint16_t minVolt = 0xFFFF;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex++) {
      uint16_t cellVolt = slaves[slaveIndex].volts[cellIndex];
      if(cellVolt < minVolt) minVolt = cellVolt;
    }
  }

  return minVolt;
}

uint16_t max_volt() {
  uint16_t maxVolt = 0x0;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex++) {
      uint16_t cellVolt = slaves[slaveIndex].volts[cellIndex];
      if(cellVolt > maxVolt) maxVolt = cellVolt;
    }
  }

  return maxVolt;
}

uint16_t avg_volt() {
  uint32_t meanVolt = 0x0;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex++)
      meanVolt += slaves[slaveIndex].volts[cellIndex];
  }

  return (meanVolt / (SLAVE_NUM * CELL_NUM));
}

uint16_t min_temp() {
  uint16_t minTemp = 0xFFFF;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex = cellIndex + 3) {
      uint16_t cellTemp = slaves[slaveIndex].temps[cellIndex];

      if(cellTemp < minTemp) minTemp = cellTemp;
    }
  }

  return minTemp;
}

uint16_t max_temp() {
  uint16_t maxTemp = 0x0;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex = cellIndex + 3) {
      uint16_t cellTemp = slaves[slaveIndex].temps[cellIndex];

      if(cellTemp > maxTemp) maxTemp = cellTemp;
    }
  }

  return maxTemp;
}

uint16_t avg_temp() {
  uint32_t meanTemp = 0x0;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex = cellIndex + 3)
      meanTemp += slaves[slaveIndex].temps[cellIndex];
  }

  return (meanTemp / (SLAVE_NUM * (CELL_NUM / 3)));
}

uint8_t max_temp_nslave() {
  uint16_t maxTemp = 0x0;
  uint8_t maxTempSlaveNum = 0x0;

  for(int slaveIndex = 0; slaveIndex < SLAVE_NUM; slaveIndex++) {
    for (int cellIndex = 0; cellIndex < CELL_NUM; cellIndex = cellIndex + 3) {
      uint16_t cellTemp = slaves[slaveIndex].temps[cellIndex];

      if(cellTemp > maxTemp){ 
        maxTemp = cellTemp;
        maxTempSlaveNum = slaveIndex;
      }
    }
  }

  return maxTempSlaveNum;
}