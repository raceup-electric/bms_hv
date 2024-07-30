#ifndef STRUCTS_H_
#define STRUCTS_H_

#include "config.h"

enum class Mode {
  NORMAL,
  SLEEP,
  BALANCE,
  STORAGE
};

struct Slave {
  uint16_t volts[CELL_NUM];
  uint16_t temps[TEMP_NUM];
  uint8_t addr;
  uint8_t err;
};

enum class FanState {
  OFF,
  RAMPING,
  ON
};

struct Fan {
  uint8_t speed;
  FanState state;
};

struct LEM {
  int32_t curr; // in mA
  uint32_t last_recv;
};

struct Precharge {
  float bus_volt;
  bool via_can;
  uint64_t start_tmstp;
  uint8_t cycle_counter;
  bool done;
};

struct SOC {
  uint32_t t_prev;
  float soc;
  float dod;
  float soh;
};

struct BMS {
  Slave slaves[SLAVE_NUM];
  uint16_t max_volt;
  uint16_t min_volt;
  uint32_t tot_volt;
  uint16_t max_temp;
  uint16_t min_temp;
  uint16_t tot_temp;
  Fan fan;
  LEM lem;
  bool sdc_closed;
  uint32_t fault_volt_tmstp;
  uint32_t fault_temp_tmstp;
  Mode mode;
  Precharge precharge;
  bool serial_gui_conn;
  uint16_t ws_gui_conn;
  SOC soc;
};

#endif // STRUCTS_H_