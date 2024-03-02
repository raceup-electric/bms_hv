#include "isospi.h"
#include "operations.h"
#include "canc.h"
#include "fan.h"
#include "soc.h"

// global bms state
BMS g_bms = {};

void setup() {
  Serial.begin(115200);
  init_spi();
  wakeup_sleep();
  init_bms();
  init_can();
  init_fan();
  init_soc();
  reset_measures();
}

void loop() { 
  update_mode();
  if (g_bms.mode == Mode::NORMAL) {
    precharge_control();
    start_adcv();
    read_volts();
    start_adax();
    read_temps();
    estimate_soc();
    set_fan_dutycycle(); 
    if (FAULT_ENABLE) {
      check_faults();
    }
    send_can();
  }
  if (g_bms.mode == Mode::BALANCE) {
    start_adcv();
    read_volts();
    start_adax();
    read_temps();
    set_fan_dutycycle(); 
    if (FAULT_ENABLE) {
      check_faults();
    }
    send_can();
    balance();
  }
  if (g_bms.gui_conn) {
   print_slaves_bin();
  }
  if (DEBUG && !g_bms.gui_conn) {
    print_slaves_hr();
  }
  reset_measures();
}
