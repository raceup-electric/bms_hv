#include "isospi.h"
#include "operations.h"
#include "canc.h"
#include "fan.h"
#include "ui.h"

// global bms state
BMS g_bms = {};
EasyNex ui(Serial3);

void setup() {
  Serial.begin(115200);
  if (SCREEN_ENABLE) {
    init_nextion();
  }
  init_spi();
  wakeup_sleep();
  init_bms();
  init_can();
  init_fan();
}
uint8_t cycle_count = 0;


void loop() { 
  update_mode();
  if (g_bms.mode == Mode::NORMAL) {
    precharge_control();
    start_adcv();
    read_volts();
    start_adax();
    read_temps();
    set_fan_dutycycle(); 
    if (FAULT_ENABLE) {
      check_faults();
    }
    send_can();
  }
  if (g_bms.gui_conn && cycle_count % 5 == 0) {
   print_slaves_bin();
  }
  cycle_count++;
  if (DEBUG && !g_bms.gui_conn) {
    print_slaves_hr();
  }
  if (SCREEN_ENABLE) {
    render();
    ui.NextionListen();
  }
  reset_measures();
}
