#include "spi.h"
#include "operations.h"

// globals
Slave slaves[SLAVE_NUM];
Mode mode;

void setup() {
  Serial.begin(115200);
  init_spi();
  wakeup_sleep();
  init();
}

void loop() { 
  update_mode();
  if (Mode::NORMAL) {
    start_adcv();
    read_volts();
    start_adax();
    read_temps();
  }
  print_slaves_hr();
}
