#include <SPI.h>

#include "config.h"
#include "bms.h"

void setup() {
  // The CS pin idles high
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);

  delay(500);

  Serial.begin(115200);
  Serial.println("Initializing...");

  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(84); // 1 MHz
  Serial.println("SPI port initialized");

  wakeup_sleep();
  Serial.println("Wakeup completed");

  init_slaves_struct();
  init_slaves_cfg();
  write_slaves_cfg();
  pwmcfg();
  write_pwmcfg();
  Serial.println("BMS slaves initialized");

  Serial.println("Starting balancing...");
  //start_balancing(0x3);
}

void loop() {

  wakeup_sleep();
  start_adcv();
  delay(MEASUREMENT_LOOP_DELAY);
  read_voltages();
  start_adax();
  delay(MEASUREMENT_LOOP_DELAY);
  read_temperatures();
  delay(MEASUREMENT_LOOP_DELAY);

  print_slaves();
}
