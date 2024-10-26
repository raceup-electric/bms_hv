#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#include <Arduino.h>

#include "config.h"
#include "canc.h"

extern BMS g_bms;

extern QueueHandle_t commands_queue;

void init_bms();

void init_cfg(Mode mode);

void start_adcv();

void read_volts();

void save_volts(int slave_idx, char reg, uint8_t* raw_volts);

void start_adax();

void read_temps();

void save_temps(int slave_idx, char reg, uint8_t* raw_temps);

void balance();

void check_faults();

Mode read_mode();

void update_mode();

void update_mode(Mode new_mode);

void print_slaves_hr();

void print_slaves_bin();

void send_can();

void reset_measures();

inline void nap() {
  esp_sleep_enable_timer_wakeup(MONITORING_SLEEP_TIMEOUT * 1000000000);
  esp_light_sleep_start();
}

#endif