#include "isospi.h"
#include "operations.h"
#include "canc.h"

// globals
Slave slaves[SLAVE_NUM] = {};
Mode mode = Mode::NORMAL;
LEM lem = { .curr = 0, .last_recv = millis()};
Precharge prech = { .bus_volt = 0.0, .via_can = false };

void setup() {
  Serial.begin(115200);
  init_spi();
  wakeup_sleep();
  init_bms();

  init_can();
}

void loop() { 
  update_mode();
  if (mode == Mode::NORMAL) {
    start_adcv();
    read_volts();
    start_adax();
    read_temps();
    print_slaves_bin();
    send_slaves_can();
  }
}
