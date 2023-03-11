#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#include <Arduino.h>

#include "config.h"

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
  bool enabled;
};

extern Slave slaves[SLAVE_NUM];
extern Mode mode;

void init_bms();

void init_pwm();

void init_cfg(Mode mode);

void start_adcv();

void read_volts();

void save_volts(int slave_idx, char reg, uint8_t* raw_volts);

void start_adax();

void read_temps();

void save_temps(int slave_idx, char reg, uint8_t* raw_temps);

void update_mode();

Mode read_mode();

void print_slaves_hr();

void print_slaves_bin();

#endif