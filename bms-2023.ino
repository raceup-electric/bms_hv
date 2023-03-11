#include <SPI.h>

#include "config.h"
#include "bms.h"

char MODE = static_cast<char>(MODES::SLEEP);
bool MODE_CHANGED = true;

void setup() {
  // The CS pin idles high
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);

  delay(500);

  Serial.begin(115200);
  //Serial.println("Initializing...");

  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(84); // 1 MHz
  //Serial.println("SPI port initialized");

  wakeup_sleep();
  //Serial.println("Wakeup completed");

  init_slaves_struct();
}

void loop() {
  if(Serial.available() > 0) {
    char newMODE = (char) Serial.read();
    MODE_CHANGED = (newMODE != MODE) ? true : false;
    MODE = static_cast<char>((MODES) newMODE);

    Serial.write(newMODE);
    delay(200);
    Serial.flush();
  }

  if(MODE_CHANGED){
    init_slaves_cfg(MODE);
    write_slaves_cfg();
    pwmcfg();
    write_pwmcfg();
    MODE_CHANGED = false;
  }

  switch(MODE) {
    case static_cast<char>(MODES::NORMAL):
      wakeup_sleep();
      start_adcv();
      delay(MEASUREMENT_LOOP_DELAY);
      read_voltages();
      start_adax();
      delay(MEASUREMENT_LOOP_DELAY);
      read_temperatures();
      delay(MEASUREMENT_LOOP_DELAY);

      Serial.write((const char*) slaves, sizeof(slaves));
      break;

    case static_cast<char>(MODES::BALANCING):
      break;
    
    default:
      break;
  }
}
