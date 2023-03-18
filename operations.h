#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#include <Arduino.h>

#include "config.h"
#include "canc.h"

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

struct BMSData {
  uint16_t max_volt;
  uint16_t min_volt;
  uint16_t avg_volt;
  uint16_t max_temp;
  uint16_t min_temp;
  uint16_t avg_temp;
  uint32_t fault_volt;
  uint32_t fault_temp;
};

extern Slave slaves[SLAVE_NUM];
extern Mode mode;
extern BMSData bms_data;

void init_bms();

void init_pwm();

void init_cfg(Mode mode);

void start_adcv();

void read_volts();

void save_volts(int slave_idx, char reg, uint8_t* raw_volts);

void start_adax();

void read_temps();

void save_temps(int slave_idx, char reg, uint8_t* raw_temps);

void update_data();

void update_mode();

Mode read_mode();

void print_slaves_hr();

void print_slaves_bin();

void send_slaves_can();

#endif