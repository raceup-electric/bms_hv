#ifndef STRUCTS_H_
#define STRUCTS_H_

#include "config.h"
#include "EasyNextionLibrary.h"

enum class Mode {
  NORMAL,
  SLEEP,
  BALANCE
};

struct Slave {
  uint16_t volts[CELL_NUM];
  uint16_t temps[3];
  uint8_t addr;
  bool err;
};

struct LEM {
  uint32_t curr;
  uint32_t last_recv;
};

struct Precharge {
  float bus_volt;
  bool via_can;
  uint32_t start_tmstp;
  bool done;
};

struct BMS {
  uint16_t max_volt;
  uint16_t min_volt;
  uint32_t tot_volt;
  uint16_t max_temp;
  uint16_t min_temp;
  uint16_t tot_temp;
  uint8_t max_temp_slave;
  bool sdc_closed;
  uint32_t fault_volt_tmstp;
  uint32_t fault_temp_tmstp;
  Slave slaves[SLAVE_NUM];
  Mode mode;
  LEM lem;
  Precharge precharge;
};

#endif // STRUCTS_H_