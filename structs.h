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
  uint16_t temps[TEMP_NUM];
  uint8_t addr;
  uint8_t err;
};

struct Fan {
  uint16_t prev_temp;
  bool on;
};

struct LEM {
  int32_t curr;
  uint32_t last_recv;
};

struct Precharge {
  float bus_volt;
  bool via_can;
  uint32_t start_tmstp;
  uint8_t cycle_counter;
  bool done;
};

struct BMS {
  Slave slaves[SLAVE_NUM];
  uint16_t max_volt;
  uint16_t min_volt;
  uint32_t tot_volt;
  uint16_t max_temp;
  uint16_t min_temp;
  uint16_t tot_temp;
  uint8_t max_temp_slave;
  Fan fan;
  LEM lem;
  bool sdc_closed;
  uint32_t fault_volt_tmstp;
  uint32_t fault_temp_tmstp;
  Mode mode;
  Precharge precharge;
  bool gui_conn;
};

#endif // STRUCTS_H_