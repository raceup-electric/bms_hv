#include "isospi.h"
#include "operations.h"
#include "canc.h"
#include "fan.h"

// global bms state
BMS g_bms = {};

void setup() {
  Serial.begin(115200);
  init_spi();
  wakeup_sleep();
  init_bms();
  init_can();
  init_fan();
}

void loop() { 
  update_mode();
  if (g_bms.mode == Mode::NORMAL) {
    precharge_control();
    start_adcv();
    read_volts();
    start_adax();
    read_temps();
    set_fan_dutycycle(); 
    check_faults();
    print_slaves_bin();
    send_can();
    reset_measures();
  }
}
