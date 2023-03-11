#include "spic.h"
#include "operations.h"

// globals
Slave slaves[SLAVE_NUM] = {};
Mode mode = Mode::NORMAL;

void setup() {
  Serial.begin(115200);
  init_spi();
  Serial.println("SPI initialized");
  wakeup_sleep();
  init_bms();
  Serial.println("BMS initialized");
}

void loop() { 
  //update_mode();
  if (mode == Mode::NORMAL) {
    start_adcv();
    read_volts();
    // start_adax();
    // read_temps();
  }
  print_slaves_hr();
}
