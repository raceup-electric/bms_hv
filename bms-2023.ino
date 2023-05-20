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
  init_nextion();
  init_spi();
  wakeup_sleep();
  init_bms();
  //init_can();
  //init_fan();
}

void loop() { 
  ui.NextionListen();
  update_mode();
  if (g_bms.mode == Mode::NORMAL) {
    //precharge_control();
    start_adcv();
    read_volts();
    start_adax();
    read_temps();
    //set_fan_dutycycle(); 
    //check_faults();
    //send_can();
    //print_slaves_hr();
  }
  render();
  reset_measures();
}
