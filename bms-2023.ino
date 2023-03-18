#include "isospi.h"
#include "operations.h"
#include "canc.h"
#include "fan.h"

// globals
Slave slaves[SLAVE_NUM] = {};
Mode mode = Mode::NORMAL;
LEM lem = { .curr = 0, .last_recv = millis()};
Precharge prech = { .bus_volt = 0.0, .via_can = false };
BMSData bms_data = {};

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
  if (mode == Mode::NORMAL) {
    start_adcv();
    read_volts();
    start_adax();
    read_temps();
    update_data();
    set_fan_dutycycle(); 
    check_faults();
    print_slaves_hr();
    send_slaves_can();
  }
}
